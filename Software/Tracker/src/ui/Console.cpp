#include "Console.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#define snprintf _snprintf
#endif

#define IM_ARRAYSIZE(_A) ((size_t)(sizeof(_A)/sizeof(*_A)))

static int Stricmp(const char* str1, const char* str2) {
  int d;
  while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) {
    str1++;
    str2++;
  }
  return d;
}

static int Strnicmp(const char* str1, const char* str2, int n) {
  int d = 0;
  while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) {
    str1++;
    str2++;
    n--;
  }
  return d;
}

static char* Strdup(const char* str) {
  size_t len = strlen(str) + 1;
  void* buff = malloc(len);
  return (char*)memcpy(buff, (const void*)str, len);
}

Console::Console() {
  ClearLog();
  memset(InputBuf, 0, sizeof(InputBuf));
  HistoryPos = -1;
  Commands.push_back("help");
  Commands.push_back("startscript");
  Commands.push_back("stop");
  Commands.push_back("speed");
  Commands.push_back("rot");
  Commands.push_back("stopvideo");
  Commands.push_back("startvideo");
  Commands.push_back("record");
  Commands.push_back("stoprecord");
  Commands.push_back("setclassifier");
}

Console::~Console() {
  ClearLog();
  for (int i = 0; i < History.Size; i++) free(History[i]);
}

void Console::ClearLog() {
	Items.clear();
  ScrollToBottom = true;
}

void Console::AddLog(const char* fmt, ...) {
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
  buf[IM_ARRAYSIZE(buf) - 1] = 0;
  va_end(args);

	if (Items.size() >= maxLines) {
		std::rotate(Items.begin(), Items.begin() + 1, Items.end());
		Items.back() = buf;
	}
	else {
		Items.emplace_back(buf);
	}

  ScrollToBottom = true;
}

const char* Console::Draw(const char* title, float w, float h) {
  if (!ImGui::BeginChild(title, ImVec2(w, h), true, ImGuiWindowFlags_NoScrollbar)) {
    ImGui::End();
    return nullptr;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
  filter.Draw("search", 180);
  ImGui::PopStyleVar();

  ImGui::BeginChild("ScrollingRegion",
                    ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false,
                    ImGuiWindowFlags_HorizontalScrollbar);
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::Selectable("Clear")) ClearLog();
    ImGui::EndPopup();
  }

  // Display every line as a separate entry so we can change their color or
  // add custom widgets. If you only want raw text you can use
  // ImGui::TextUnformatted(log.begin(), log.end());
  // NB- if you have thousands of entries this approach may be too inefficient
  // and may require user-side clipping to only process visible items.
  // You can seek and display only the lines that are visible using the
  // ImGuiListClipper helper, if your elements are evenly spaced and you have
  // cheap random access to the elements.
  // To use the clipper we could replace the 'for (int i = 0; i < Items.Size;
  // i++)' loop with:
  //     ImGuiListClipper clipper(Items.Size);
  //     while (clipper.Step())
  //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
  // However take note that you can not use this code as is if a filter is
  // active because it breaks the 'cheap random-access' property. We would
  // need random-access on the post-filtered list.
  // A typical application wanting coarse clipping and filtering may want to
  // pre-compute an array of indices that passed the filtering test,
  // recomputing this array when user changes the filter,
  // and appending newly elements as they are inserted. This is left as a task
  // to the user until we can manage to improve this example code!
  // If your items are of variable size you may want to implement code similar
  // to what ImGuiListClipper does. Or split your data into fixed height items
  // to allow random-seeking into your list.
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                      ImVec2(4, 1));  // Tighten spacing
  for (size_t i = 0; i < Items.size(); i++) {
    const char* item = Items[i].c_str();
    if (!filter.PassFilter(item)) continue;
    ImVec4 col =
        ImVec4(1.f, 1.f, 1.f, 1.0f);  // A better implementation may store
                                         // a type per-item. For the sample
                                         // let's just parse the text.
    if (strstr(item, "[error]"))
      col = ImColor(221, 87, 81);
    else if (strncmp(item, "# ", 2) == 0)
      col = ImColor(221, 87, 81);
    ImGui::PushStyleColor(ImGuiCol_Text, col);
    ImGui::TextUnformatted(item);
    ImGui::PopStyleColor();
  }
  if (ScrollToBottom) ImGui::SetScrollHere();
  ScrollToBottom = false;
  ImGui::PopStyleVar();
  ImGui::EndChild();
  ImGui::Separator();

  const auto cb = [](ImGuiTextEditCallbackData* data) {
    Console* console = (Console*)data->UserData;
    return console->TextEditCallback(data);
  };

  const char* result = nullptr;
  if (ImGui::InputText("##input", InputBuf, IM_ARRAYSIZE(InputBuf),
                       ImGuiInputTextFlags_EnterReturnsTrue |
                           ImGuiInputTextFlags_CallbackCompletion |
                           ImGuiInputTextFlags_CallbackHistory,
                       cb, (void*)this)) {
    char* input_end = InputBuf + strlen(InputBuf);
    while (input_end > InputBuf && input_end[-1] == ' ') input_end--;
    *input_end = 0;
    if (InputBuf[0]) {
      const char* userCommand = ExecCommand(InputBuf);
      if (userCommand) {
        if (lastCommand) free(lastCommand);
        lastCommand = strdup(userCommand);
        result = lastCommand;
      }
    }
    strcpy(InputBuf, "");
  }

  // Demonstrate keeping auto focus on the input box
  if (ImGui::IsItemHovered() ||
      (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() &&
       !ImGui::IsMouseClicked(0)))
    ImGui::SetKeyboardFocusHere(-1);  // Auto focus previous widget

  ImGui::End();

  return result;
}

const char* Console::ExecCommand(const char* command_line) {
  AddLog("# %s\n", command_line);

  // Insert into history. First find match and delete it so it can be pushed
  // to the back. This isn't trying to be smart or optimal.
  HistoryPos = -1;
  for (int i = History.Size - 1; i >= 0; i--)
    if (Stricmp(History[i], command_line) == 0) {
      free(History[i]);
      History.erase(History.begin() + i);
      break;
    }
  History.push_back(Strdup(command_line));

  // Process command
  if (Stricmp(command_line, "CLEAR") == 0) {
    ClearLog();
  } else if (Stricmp(command_line, "HELP") == 0) {
    AddLog("Commands:");
    for (int i = 0; i < Commands.Size; i++) AddLog("- %s", Commands[i]);
  } else {
    return command_line;
  }

  return nullptr;
}

int Console::TextEditCallback(ImGuiTextEditCallbackData* data) {
  // AddLog("cursor: %d, selection: %d-%d", data->CursorPos,
  // data->SelectionStart, data->SelectionEnd);
  switch (data->EventFlag) {
    case ImGuiInputTextFlags_CallbackCompletion: {
      // Locate beginning of current word
      const char* word_end = data->Buf + data->CursorPos;
      const char* word_start = word_end;
      while (word_start > data->Buf) {
        const char c = word_start[-1];
        if (c == ' ' || c == '\t' || c == ',' || c == ';') break;
        word_start--;
      }

      // Build a list of candidates
      ImVector<const char*> candidates;
      for (int i = 0; i < Commands.Size; i++)
        if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) ==
            0)
          candidates.push_back(Commands[i]);

      if (candidates.Size == 0) {
        // No match
        AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start),
               word_start);
      } else if (candidates.Size == 1) {
        // Single match. Delete the beginning of the word and replace it
        // entirely so we've got nice casing
        data->DeleteChars((int)(word_start - data->Buf),
                          (int)(word_end - word_start));
        data->InsertChars(data->CursorPos, candidates[0]);
        data->InsertChars(data->CursorPos, " ");
      } else {
        // Multiple matches. Complete as much as we can, so inputing "C" will
        // complete to "CL" and display "CLEAR" and "CLASSIFY"
        int match_len = (int)(word_end - word_start);
        for (;;) {
          int c = 0;
          bool all_candidates_matches = true;
          for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
            if (i == 0)
              c = toupper(candidates[i][match_len]);
            else if (c != toupper(candidates[i][match_len]))
              all_candidates_matches = false;
          if (!all_candidates_matches) break;
          match_len++;
        }

        if (match_len > 0) {
          data->DeleteChars((int)(word_start - data->Buf),
                            (int)(word_end - word_start));
          data->InsertChars(data->CursorPos, candidates[0],
                            candidates[0] + match_len);
        }

        // List matches
        AddLog("Possible matches:\n");
        for (int i = 0; i < candidates.Size; i++)
          AddLog("- %s\n", candidates[i]);
      }

      break;
    }
    case ImGuiInputTextFlags_CallbackHistory: {
      const int prev_history_pos = HistoryPos;
      if (data->EventKey == ImGuiKey_UpArrow) {
        if (HistoryPos == -1)
          HistoryPos = History.Size - 1;
        else if (HistoryPos > 0)
          HistoryPos--;
      } else if (data->EventKey == ImGuiKey_DownArrow) {
        if (HistoryPos != -1)
          if (++HistoryPos >= History.Size) HistoryPos = -1;
      }

      // A better implementation would preserve the data on the current input
      // line along with cursor position.
      if (prev_history_pos != HistoryPos) {
        data->CursorPos = data->SelectionStart = data->SelectionEnd =
            data->BufTextLen =
                (int)snprintf(data->Buf, (size_t)data->BufSize, "%s",
                              (HistoryPos >= 0) ? History[HistoryPos] : "");
        data->BufDirty = true;
      }
    }
  }
  return 0;
}
