#pragma once
#include <imgui.h>

struct Console {
  char InputBuf[512];
  ImVector<char*> Items;
  bool ScrollToBottom;
  ImVector<char*> History;
  int HistoryPos;  // -1: new line, 0..History.Size-1 browsing history.
  ImVector<const char*> Commands;
  ImGuiTextFilter filter;
  char* lastCommand = nullptr;

  Console();
  ~Console();

  void ClearLog();

  void AddLog(const char* fmt, ...) IM_PRINTFARGS(2);
  const char* Draw(const char* title, float w, float h);
  const char* ExecCommand(const char* command_line);
  int TextEditCallback(ImGuiTextEditCallbackData* data);
};
