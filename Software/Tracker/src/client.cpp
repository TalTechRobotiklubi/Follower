#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <enet/enet.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Texture.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "proto/frame_generated.h"
#include "Decode.h"
#include "AABB.h"

struct ClientOptions {
  const char* host = "127.0.0.1";
  uint16_t port = 9001;

  int screenWidth = 1280;
  int screenHeight = 720;
};

struct Client {
  Decoder* decoder = nullptr;
  ENetHost* udpClient = nullptr;
  ENetPeer* peer = nullptr;
  Texture decodedDepth;
  std::vector<AABB> detections;

  ~Client();
};

Client::~Client() {
  enet_host_destroy(udpClient);
  enet_deinitialize();
}

bool ClientStart(Client* c, const ClientOptions* opt) {
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

  ENetAddress address;
  enet_address_set_host(&address, opt->host);
  address.port = opt->port;

  c->peer = enet_host_connect(c->udpClient, &address, 1, 0);

  if (!c->peer) {
    printf("failed to initialize connection\n");
    return false;
  }

  return true;
}

void ClientUpdate(Client* c) {
  ENetEvent event;
  if (enet_host_service(c->udpClient, &event, 0) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        printf("connected\n");
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("disconnect\n");
        break;
      case ENET_EVENT_TYPE_RECEIVE: {
        const proto::Frame* frame = proto::GetFrame(event.packet->data);

        rgba_image img;
        if (DecodeFrame(c->decoder, frame->depth()->Data(), frame->depth()->size(), 512, 424, &img)) {
          TextureUpdate(&c->decodedDepth, img.data, img.width, img.height);
        }

        c->detections.clear();
        auto detections = frame->detections();
        for (size_t i = 0; i < detections->size(); i++) {
          const proto::Detection* d = detections->Get(i);
          if (d->weight() >= 1.f) {
            AABB box;
            box.top_left.x = float(d->x());
            box.top_left.y = float(d->y());
            box.bot_right.x = box.top_left.x + float(d->width());
            box.bot_right.y = box.top_left.y + float(d->height());
            c->detections.push_back(box);
          }
        }

        enet_packet_destroy(event.packet);
        break;
      }
      default:
        printf("Enet unhandled\n");
        break;
    }
  }
}

void glfwError(int error, const char* description) {
  fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

int main(int argc, char** argv) {
  glfwSetErrorCallback(glfwError);

  if (!glfwInit()) return 1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  GLFWwindow* window = glfwCreateWindow(mode->width, mode->height,
                                        "Follower Remote", nullptr, nullptr);

  glfwMakeContextCurrent(window);

  if (gl3wInit()) {
    fprintf(stderr, "Failed to initialize OpenGL\n");
    return 1;
  }

  ImGui_ImplGlfwGL3_Init(window, true);

  // ImGui::GetStyle() = loadStyle(ImGui::GetStyle());

  bool showWindow = true;
  ImVec4 bgColor = ImColor(218, 223, 225);

  ClientOptions options;
  Client client;
  if (!ClientStart(&client, &options)) {
    return 1;
  }

  while (!glfwWindowShouldClose(window)) {
    ClientUpdate(&client);
    glfwPollEvents();

    if (ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) break;

    ImGui_ImplGlfwGL3_NewFrame();
    int displayWidth, displayHeight;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;

    ImGui::Begin("follower", &showWindow,
                 ImVec2(float(displayWidth), float(displayHeight)), -1.f,
                 flags);

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        ImGui::MenuItem("DUMMY", NULL, false, false);
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }

    ImGui::Text("main");

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::Image(client.decodedDepth.PtrHandle(),
                 ImVec2(client.decodedDepth.width, client.decodedDepth.height));

    ImU32 rect_color = ImColor(240, 240, 20);
    for (AABB& box : client.detections) {
			const float x = p.x + box.top_left.x;
			const float y = p.y + box.top_left.y;
			const float w = box.bot_right.x - box.top_left.x;
			const float h = box.bot_right.y - box.top_left.y;
			ImVec2 points[4] = {
				ImVec2(x, y),
				ImVec2(x + w, y),
				ImVec2(x + w, y + h),
				ImVec2(x, y + h)
			};
			draw_list->AddPolyline(points, 4, rect_color, true, 4.f, true);
    }
    ImGui::End();

    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();

  return 0;
}
