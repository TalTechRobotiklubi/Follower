#include <GLFW/glfw3.h>
#include <assert.h>
#include <enet/enet.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constants.h"
#include "CoreObj.h"
#include "Decode.h"
#include "File.h"
#include "Style.h"
#include "Texture.h"
#include "fl_math.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "parg/parg.h"
#include "proto/message_generated.h"
#include "ui/Console.h"

struct ClientOptions {
  const char* host = "127.0.0.1";
  uint16_t port = 9001;

  int screenWidth = 1280;
  int screenHeight = 720;
};

std::vector<std::string> split(const std::string& text, char sep) {
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}

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

void ShiftPush(std::vector<float>& container, float v) {
  if (container.size() < 1) return;

  std::rotate(container.begin(), container.begin() + 1, container.end());
  container.back() = v;
}

struct Client {
  ControlState state;
  TrackingState tracking;
  World* world = nullptr;
  Decoder* decoder = nullptr;
  ENetHost* udpClient = nullptr;
  ENetPeer* peer = nullptr;
  Texture decodedDepth;
  double coreTimestamp = 0.0;
  bool connected = false;
	bool debugWindow = false;
  const ClientOptions* options;
  Console* console = nullptr;
  std::vector<float> speedHistory = std::vector<float>(512, 0.f);
  std::vector<float> rotationSpeedHistory = std::vector<float>(512, 0.f);
  std::vector<float> frameTimeHistory = std::vector<float>(256, 0.f);
	std::vector<float> frameSizeHistory = std::vector<float>(256, 0.f);
	std::vector<Texture> candidateImages;

	Client();
  ~Client();
};

void ClientSendData(Client* c, const uint8_t* data, size_t len) {
  if (!c->peer) {
    printf("unable to send command - no connection\n");
    return;
  }

  ENetPacket* packet = enet_packet_create(data, len, ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(c->peer, 0, packet);
  enet_host_flush(c->udpClient);
}

void SendCommand(Client* c, proto::CommandType type, const char* args) {
  flatbuffers::FlatBufferBuilder builder;
  auto cmd = proto::CreateCommandDirect(builder, type, args);
  auto message =
      proto::CreateMessage(builder, proto::Payload_Command, cmd.Union());
  builder.Finish(message);
  ClientSendData(c, builder.GetBufferPointer(), builder.GetSize());
}

void HandleCommand(Client* c, const std::vector<std::string>& tokens) {
  const std::string& command = tokens.front();
  Console* console = c->console;

  const auto needArg = [&](size_t n) {
    if (tokens.size() < n) {
      console->AddLog("need an argument");
      return true;
    }
    return false;
  };

  if (command == "startscript") {
    if (needArg(1)) return;

    const std::string& file = tokens[1];

    IoVec content = LoadFile(file.c_str());

    if (!content.data) {
      console->AddLog("no such file");
      return;
    }

    console->AddLog("sending %s [%zu bytes]", file.c_str(), content.len);

    flatbuffers::FlatBufferBuilder builder;
    auto script = proto::CreateLuaMainScript(
        builder, builder.CreateString((const char*)content.data, content.len));
    auto message = proto::CreateMessage(builder, proto::Payload_LuaMainScript,
                                        script.Union());
    builder.Finish(message);
    ClientSendData(c, builder.GetBufferPointer(), builder.GetSize());

    free(content.data);
  } else if (command == "stop") {
    SendCommand(c, proto::CommandType_Stop, nullptr);
  } else if (command == "speed") {
    if (needArg(1)) return;
    SendCommand(c, proto::CommandType_Speed, tokens[1].c_str());
  } else if (command == "rot") {
    if (needArg(1)) return;
    SendCommand(c, proto::CommandType_RotationSpeed, tokens[1].c_str());
  } else if (command == "stopvideo") {
    SendCommand(c, proto::CommandType_StopVideo, nullptr);
  } else if (command == "startvideo") {
    SendCommand(c, proto::CommandType_StartVideo, nullptr);
  } else if (command == "record") {
    SendCommand(c, proto::CommandType_RecordDepth, nullptr);
  } else if (command == "stoprecord") {
    SendCommand(c, proto::CommandType_StopRecord, nullptr);
	} else if (command == "startdebug") {
		SendCommand(c, proto::CommandType_StartDebug, nullptr);
		c->debugWindow = true;
	} else if (command == "stopdebug") {
		SendCommand(c, proto::CommandType_StopDebug, nullptr);
		c->debugWindow = false;
	} else if (command == "setclassifier") {
    if (needArg(1)) return;

    const std::string& inputFile = tokens[1];

    IoVec content = LoadFile(inputFile.c_str());

    if (!content.data) {
      console->AddLog("no such file");
      return;
    }

    console->AddLog("sending %s [%zu bytes]", inputFile.c_str(), content.len);

    flatbuffers::FlatBufferBuilder builder;
    auto classifier = proto::CreateClassifier(
        builder, builder.CreateString("core_current_classifier.nn"),
        builder.CreateVector((const int8_t*)content.data, content.len));
    auto message = proto::CreateMessage(builder, proto::Payload_Classifier,
                                        classifier.Union());
    builder.Finish(message);
    ClientSendData(c, builder.GetBufferPointer(), builder.GetSize());

    free(content.data);
  }
}

Client::Client()
	: world((World*)calloc(1, sizeof(World))) {}

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

void ClientHandleFrame(Client* c, const proto::Frame* frame) {
  const proto::Vec2* cam = frame->camera();
  c->state.camera.x = cam->x();
  c->state.camera.y = cam->y();
  c->state.rotationSpeed = frame->rotationSpeed();
  c->state.speed = frame->speed();
  c->coreTimestamp = frame->timestamp();

  ShiftPush(c->frameTimeHistory, frame->coreDtMs());
  ShiftPush(c->rotationSpeedHistory, frame->rotationSpeed());
  ShiftPush(c->speedHistory, frame->speed());

  if (frame->depth()) {
    RgbaImage img;
    if (DecodeFrame(c->decoder, frame->depth()->Data(), frame->depth()->size(),
                    kDepthWidth, kDeptHeight, &img)) {
      TextureUpdate(&c->decodedDepth, img.data, img.width, img.height);
    }
  }

	World* world = c->world;
	world->numDetections = int32_t(frame->detections()->size());
	
  auto detections = frame->detections();
  for (uint32_t i = 0; i < detections->size(); i++) {
		const proto::Detection* d = detections->Get(i);
		Detection* local = &world->detections[i];
    local->depthTopLeft.x = d->depthTopLeft()->x();
    local->depthTopLeft.y = d->depthTopLeft()->y();
    local->depthBotRight.x = d->depthBotRight()->x();
    local->depthBotRight.y = d->depthBotRight()->y();
    const proto::Vec3* metric = d->metricPosition();
    local->metricPosition = vec3(metric->x(), metric->y(), metric->z());
    local->weight = d->weight();

		if (d->histogram()) {
			std::copy(d->histogram()->begin(), d->histogram()->end(), &local->histogram[0]);
		}

		if (d->png()) {
			if (i >= c->candidateImages.size()) {
				c->candidateImages.push_back(Texture());
			}
			TextureUpdate(&c->candidateImages[i], d->png()->data(), kCandidateWidth, kCandidateHeight);
		}
  }

  auto tracking = frame->tracking();
  auto targets = tracking->targets();
  c->tracking.numTargets = tracking->targets()->size();
  c->tracking.activeTarget = tracking->activeTarget();
  for (uint32_t i = 0; i < targets->size(); i++) {
    const proto::Target* t = targets->Get(i);
    c->tracking.targets[i] =
        Target(t->weight(), vec2{t->kinect().x(), t->kinect().y()},
               vec3{t->position().x(), t->position().y(), t->position().z()});
  }
}

void ClientHandleStatusMessage(Client* c, const proto::StatusMessage* message) {
  c->console->AddLog("%s", message->message()->c_str());
}

void ClientHandleMessage(Client* c, const uint8_t* data, size_t len) {
  (void)len;
  const proto::Message* message = proto::GetMessage(data);
  switch (message->payload_type()) {
    case proto::Payload_Frame:
      ClientHandleFrame(c, (const proto::Frame*)message->payload());
      break;
    case proto::Payload_StatusMessage:
      ClientHandleStatusMessage(
          c, (const proto::StatusMessage*)message->payload());
      break;
      break;
    default:
      break;
  }
}

void ClientUpdate(Client* c) {
  ENetEvent event;
  while (enet_host_service(c->udpClient, &event, 0) > 0) {
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
        ClientHandleMessage(c, event.packet->data, event.packet->dataLength);
				ShiftPush(c->frameSizeHistory, float(event.packet->dataLength) / 1000.f);
        enet_packet_destroy(event.packet);
        break;
      }
      default:
        break;
    }
  }
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
  const float camRotRad = deg_to_rad(client->state.camera.x);
  const vec2 camTopLeft = vec2_rotate(vec2{-12.f, 0.f}, camRotRad);
  const vec2 camBotRight = vec2_rotate(vec2{12.f, 0.f}, camRotRad);
  drawList->AddLine(ImVec2(camTopLeft.x + robot.x, camTopLeft.y + robot.y),
                    ImVec2(camBotRight.x + robot.x, camBotRight.y + robot.y),
                    ImColor(0xC6, 0xC7, 0xC5), 10.f);

  const float radius = 12.f;
	for (int i = 0; i < client->world->numDetections; i++) {
		const Detection* detection = &client->world->detections[i];
    const float d =
        fl_map_range(detection->metricPosition.z, 0.f, 4.5f, 0.f, height);
    const float centerX =
        float(detection->depthBotRight.x - detection->depthTopLeft.x);
    const float tx = s * centerX / w;
    drawList->AddCircle(ImVec2(c.x + w * tx, robot.y - d), radius,
                        ImColor(0x66, 0xA2, 0xC6), 32);
  }

  const TrackingState* tracking = &client->tracking;
  const auto TargetToRenderCoords = [&](const Target& t) {
    const float d = fl_map_range(t.position.z, 0.f, 4.5f, 0.f, height);
    const float tx = s * t.kinect.x / w;
    return ImVec2(c.x + w * tx, robot.y - d);
  };

  for (int32_t i = 0; i < tracking->numTargets; i++) {
    const Target& t = tracking->targets[i];
    ImVec2 renderCoord = TargetToRenderCoords(t);
    drawList->AddCircleFilled(renderCoord, t.weight * radius * 0.9f,
                              ImColor(0xFF, 0xA2, 0xC6), 32);
  }

  if (tracking->activeTarget > -1) {
    const Target& t = tracking->targets[tracking->activeTarget];
    ImVec2 position = TargetToRenderCoords(t);
    drawList->AddLine(robot, position, ImColor(0xB0, 0x06, 0xEF), 2.f);
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
  bool showConsole = true;
  ImVec4 bgColor = ImColor(218, 223, 225);

  ClientOptions options = ParseOptions(argc, argv);
  Client client;

	for (int i = 0; i < 8; i++) {
		client.candidateImages.push_back(TextureAllocate(kCandidateWidth, kCandidateHeight));
	}

  if (!ClientStart(&client, &options)) {
    return 1;
  }

	client.console = new Console({
		"startscript",
		"stop",
		"speed",
		"rot",
		"stopvideo",
		"startvideo",
		"record",
		"stoprecord",
		"setclassifier",
		"startdebug",
		"stopdebug"
	});

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
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("follower", &showWindow,
                 ImVec2(float(displayWidth), float(displayHeight)), -1.f,
                 flags);

    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImGui::Text("%s:%u - %s", options.host, options.port,
                client.connected ? "connected" : "disconnected");
    ImGui::SameLine();
    ImGui::Text("| core time: %.2f", client.coreTimestamp / 1000.0);
    cursor = ImGui::GetCursorScreenPos();
    ImGui::Image(client.decodedDepth.PtrHandle(),
                 ImVec2(float(kDepthWidth), float(kDeptHeight)), ImVec2(1, 0),
                 ImVec2(0, 1));

		for (int i = 0; i < client.world->numDetections; i++) {
			const Detection* d = &client.world->detections[i];
      ImVec2 a =
          ImVec2(cursor.x + d->depthTopLeft.x, cursor.y + d->depthTopLeft.y);
      ImVec2 b =
          ImVec2(cursor.x + d->depthBotRight.x, cursor.y + d->depthBotRight.y);
      draw_list->AddRect(a, b, ImColor(255, 0, 0), 0.f, 0x0F, 2.f);
      draw_list->AddRectFilled(ImVec2(a.x + 1.f, a.y + 1.f),
                               ImVec2(b.x - 1.f, b.y - 1.f),
                               ImColor(255, 0, 255, 20));
    }
    ImGui::SameLine();

    RenderOverview(&client);

    ImGui::SameLine();

    const ImVec2 plotSize(300.f, 100.f);
    ImGui::BeginGroup();
    ImGui::PlotLines("##rotationSpeed", client.rotationSpeedHistory.data(),
                     client.rotationSpeedHistory.size(), 0, "rotation speed",
                     -360.f, 360.f, plotSize);
    ImGui::PlotLines("##speed", client.speedHistory.data(),
                     client.speedHistory.size(), 0, "speed", 0.f, 300.f,
                     plotSize);
    ImGui::PlotLines("##coreFrameTime", client.frameTimeHistory.data(),
                     client.frameTimeHistory.size(), 0, "core frame time (ms)",
                     0.f, 100.f, plotSize);
		ImGui::PlotLines("##frameSize", client.frameSizeHistory.data(), client.frameSizeHistory.size(), 0, "frame size (KB)", FLT_MAX, FLT_MAX, plotSize);
    ImGui::EndGroup();

    ImGui::Begin("##consolewindow", &showConsole, ImVec2(600.f, 400.f));
    const char* cmd =
        client.console->Draw("console", float(displayWidth - 20) * 0.5f, 300.f);
    if (cmd) {
      auto tokens = split(cmd, ' ');
      if (tokens.size() >= 1) {
        HandleCommand(&client, tokens);
      }
    }
    ImGui::End();

		if (client.debugWindow) {
			ImGui::Begin("Debug", &client.debugWindow, ImVec2(500.f, 800.f));
			for (int i = 0; i < client.world->numDetections; i++) {
				Texture& t = client.candidateImages[i];
				ImGui::Image(t.PtrHandle(), ImVec2(float(kCandidateWidth) * 2.f, float(kCandidateHeight) * 2.f));
				ImGui::SameLine();
				ImGui::PushID(i);
				ImGui::PlotHistogram("##histo", &client.world->detections[i].histogram[0], 3 * 256, 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(768.f, 256.f));
				ImGui::PopID();
			}
			ImGui::End();
		}

    ImGui::End();

    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  delete client.console;

  ImGui_ImplGlfw_Shutdown();
  glfwTerminate();

  return 0;
}
