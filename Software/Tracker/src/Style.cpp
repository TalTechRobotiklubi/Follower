#include "Style.h"

namespace Style {

ImGuiStyle Bright() {
  ImGuiStyle style;
  style.Alpha = 1.0f;
  style.WindowRounding = 0.f;
  style.FrameRounding = 0.f;
  return style;
}
}
