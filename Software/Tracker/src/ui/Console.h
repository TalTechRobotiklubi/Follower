#pragma once
#include <imgui.h>
#include <string>
#include <vector>

struct Console {
  char InputBuf[512];
  std::vector<std::string> Items;
  bool ScrollToBottom;
  ImVector<char*> History;
  int HistoryPos;  // -1: new line, 0..History.Size-1 browsing history.
  ImVector<const char*> Commands;
  ImGuiTextFilter filter;
  char* lastCommand = nullptr;
  size_t maxLines = 512;

  Console(const std::vector<const char*>& commands = {});
  ~Console();

  void ClearLog();

  void AddLog(const char* fmt, ...) IM_PRINTFARGS(2);
  const char* Draw(const char* title, float w, float h);
  const char* ExecCommand(const char* command_line);
  int TextEditCallback(ImGuiTextEditCallbackData* data);
};
