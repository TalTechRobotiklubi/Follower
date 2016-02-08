#include "ui_layout.h"
#include "fl_constants.h"

namespace Fl {

void ui_layout_calculate(ui_layout* layout, int win_width,
  int win_height) {
  layout->win_width = win_width;
  layout->win_height = win_height;
  layout->win_widthf = float(win_width);
  layout->win_heightf = float(win_height);

  layout->toolbar_width = 200.f;
  layout->plot_height = 200.f;
  layout->depth_bar_height = 50.f;

  layout->kinect_image_width = 512.f;
  layout->kinect_image_height = 424.f;

  layout->depth_bar_width =
    layout->kinect_image_width / float(fl::DEPTH_RANGE_INTERVALS);
}

}