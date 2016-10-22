#include <GLFW/glfw3.h>
#include <assert.h>
#include <enet/enet.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AABB.h"
#include "Decode.h"
#include "Style.h"
#include "Target.h"
#include "Texture.h"
#include "fl_constants.h"
#include "fl_math.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "parg/parg.h"
#include "proto/frame_generated.h"

struct CoreState {
  vec2 camera;
};

struct ClientOptions {
  const char* host = "127.0.0.1";
  uint16_t port = 9001;

  int screenWidth = 1280;
  int screenHeight = 720;
};

ClientOptions ParseOptions(int argc, char** argv) {
  ClientOptions opts;
  parg_state args;
  parg_init(&args);

  int res = -1;
  while ((res = parg_getopt(&args, argc, argv, "h:p:")) != -1) {
    switch (res) {
      case 'h':
        opts.host = args.optarg;
        break;
      case 'p':
        opts.port = atoi(args.optarg);
        break;
      default:
        break;
    }
  }

  return opts;
}

struct Detection {
  vec2 position;
  vec3 metric;
};

struct Client {
  CoreState state;
  Decoder* decoder = nullptr;
  ENetHost* udpClient = nullptr;
  ENetPeer* peer = nullptr;
  Texture decodedDepth;
  std::vector<Detection> detections;
  std::vector<Target> targets;
  double coreTimestamp = 0.0;
  bool connected = false;
  const ClientOptions* options;
  std::vector<char> commandBuffer;

  ~Client();
};

Client::~Client() {
  enet_host_destroy(udpClient);
  enet_deinitialize();
}

bool ClientStartConnection(Client* c) {
  ENetAddress address;
  enet_address_set_host(&address, c->options->host);
  address.port = c->options->port;

  c->peer = enet_host_connect(c->udpClient, &address, 1, 0);

  if (!c->peer) {
    printf("failed to initialize connection\n");
    return false;
  }

  enet_peer_timeout(c->peer, 8, 500, 2000);

  return true;
}

bool ClientStart(Client* c, const ClientOptions* opt) {
  c->options = opt;
  c->commandBuffer.resize(4096, 0);

  if (enet_initialize() != 0) {
    printf("Failed to initialize enet\n");
    return false;
  }

  c->decoder = DecoderCreate();

  if (!c->decoder) {
    printf("Failed to create decoder\n");
    return false;
  }

  c->udpClient = enet_host_create(nullptr, 1, 1, 0, 0);

  if (!c->udpClient) {
    printf("An error occurred while trying to create an ENet client host.\n");
    return false;
  }

  ClientStartConnection(c);

  return true;
}

void ClientUpdate(Client* c) {
  ENetEvent event;
  if (enet_host_service(c->udpClient, &event, 0) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        c->connected = true;
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        enet_peer_reset(c->peer);
        ClientStartConnection(c);
        c->connected = false;
        break;
      case ENET_EVENT_TYPE_RECEIVE: {
        const proto::Frame* frame = proto::GetFrame(event.packet->data);
        const proto::Vec2* cam = frame->camera();
        if (cam) {
          c->state.camera.x = cam->x();
          c->state.camera.y = cam->y();
        }
        c->coreTimestamp = frame->timestamp();
        rgba_image img;
        if (DecodeFrame(c->decoder, frame->depth()->Data(),
                        frame->depth()->size(), kDepthWidth, kDeptHeight,
                        &img)) {
          TextureUpdate(&c->decodedDepth, img.data, img.width, img.height);
        }

        c->detections.clear();
        auto detections = frame->detections();
        for (size_t i = 0; i < detections->size(); i++) {
          const proto::Detection* d = detections->Get(i);
          if (d->weight() >= 1.f) {
            Detection local;
            const proto::Vec2 position = d->position();
            local.position.x = position.x();
            local.position.y = position.y();
            const proto::Vec3 metric = d->metricPosition();
            local.metric = vec3(metric.x(), metric.y(), metric.z());
            c->detections.push_back(local);
          }
        }

        c->targets.clear();
        auto targets = frame->targets();
        for (size_t i = 0; i < targets->size(); i++) {
          const proto::Target* t = targets->Get(i);
          c->targets.emplace_back(
              t->timeToLive(), vec2{t->position().x(), t->position().y()},
              vec3{t->metricPosition().x(), t->metricPosition().y(),
                   t->metricPosition().z()});
        }

        enet_packet_destroy(event.packet);
        break;
      }
      default:
        break;
    }
  }
}

void ClientSendCommand(Client* c, const char* command, size_t len) {
  if (!c->peer) {
    printf("unable to send command - no connection\n");
    return;
  }

  ENetPacket* packet =
      enet_packet_create(command, len, ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(c->peer, 0, packet);
  enet_host_flush(c->udpClient);
}

void RenderOverview(Client* client) {
  const float s = 1.25f;
  const float w = float(kDepthWidth) * s;
  const float h = float(kDeptHeight) * s;
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImVec2 c = ImGui::GetCursorScreenPos();
  ImVec2 end = ImVec2(c.x + w, c.y + h);
  ImVec2 robot = ImVec2(c.x + w * 0.5f, c.y + h - 25.f);
  float height = robot.y - c.y;
  ImGui::Dummy(ImVec2(w, h));
  drawList->PushClipRect(c, end);
  drawList->AddRectFilled(c, end, ImColor(0x3E, 0x3E, 0x41));
  drawList->AddCircle(robot, 20.f, ImColor(0x8E, 0x8A, 0x71), 9, 2.f);

  // Camera position
  const float camRotRad = deg_to_rad(client->state.camera.y);
  const vec2 camTopLeft = vec2_rotate(vec2{-12.f, 0.f}, camRotRad);
  const vec2 camBotRight = vec2_rotate(vec2{12.f, 0.f}, camRotRad);
  drawList->AddLine(ImVec2(camTopLeft.x + robot.x, camTopLeft.y + robot.y),
                    ImVec2(camBotRight.x + robot.x, camBotRight.y + robot.y),
                    ImColor(0xC6, 0xC7, 0xC5), 10.f);

  const float radius = 12.f;
  for (const Detection& detection : client->detections) {
    const float d = fl_map_range(detection.metric.z, 0.f, 4.5f, 0.f, height);
    const float tx = s * detection.position.x / w;
    drawList->AddCircle(ImVec2(c.x + w * tx, robot.y - d), radius,
                        ImColor(0x66, 0xA2, 0xC6), 32);
  }

  for (const Target& t : client->targets) {
    const float d = fl_map_range(t.metricPosition.z, 0.f, 4.5f, 0.f, height);
    const float tx = s * t.kinectPosition.x / w;
    drawList->AddCircleFilled(ImVec2(c.x + w * tx, robot.y - d),
                              t.timeToLive / 3.0f * radius,
                              ImColor(0xFF, 0xA2, 0xC6), 32);
  }

  drawList->PopClipRect();
}

void glfwError(int error, const char* description) {
  fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

int main(int argc, char** argv) {
  glfwSetErrorCallback(glfwError);

  if (!glfwInit()) return 1;

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  GLFWwindow* window = glfwCreateWindow(mode->width, mode->height,
                                        "Follower Remote", nullptr, nullptr);

  glfwMakeContextCurrent(window);

  ImGui_ImplGlfw_Init(window, true);
  ImGui::GetStyle() = Style::Bright();

  bool showWindow = true;
  ImVec4 bgColor = ImColor(218, 223, 225);

  ClientOptions options = ParseOptions(argc, argv);
  Client client;
  if (!ClientStart(&client, &options)) {
    return 1;
  }

  while (!glfwWindowShouldClose(window)) {
    ClientUpdate(&client);
    glfwPollEvents();

    if (ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) break;

    ImGui_ImplGlfw_NewFrame();
    int displayWidth, displayHeight;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;

    ImGui::Begin("follower", &showWindow,
                 ImVec2(float(displayWidth), float(displayHeight)), -1.f,
                 flags);

    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImGui::Text("%s:%u - %s", options.host, options.port,
                client.connected ? "connected" : "disconnected");
    ImGui::SameLine();
    ImGui::Text("| core time: %.2f", client.coreTimestamp / 1000.0);
    ImGui::SameLine();
    ImGui::Text("| camera: (%.1f, %.1f)", client.state.camera.x,
                client.state.camera.y);
    cursor = ImGui::GetCursorScreenPos();
    ImGui::Image(client.decodedDepth.PtrHandle(),
                 ImVec2(kDepthWidth, kDeptHeight), ImVec2(1, 0), ImVec2(0, 1));

    for (Detection& d : client.detections) {
      draw_list->AddCircleFilled(
          ImVec2(cursor.x + d.position.x, cursor.y + d.position.y), 20.f,
          ImColor(255, 0, 0));
    }

    ImGui::SameLine();
    RenderOverview(&client);

    if (ImGui::InputText("commands", &client.commandBuffer[0],
                         client.commandBuffer.size(),
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
      size_t length = strlen(client.commandBuffer.data());
      ClientSendCommand(&client, client.commandBuffer.data(), length);
    }

    ImGui::End();

    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  ImGui_ImplGlfw_Shutdown();
  glfwTerminate();

  return 0;
}
