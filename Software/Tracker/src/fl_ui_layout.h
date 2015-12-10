#ifndef FL_UI_LAYOUT
#define FL_UI_LAYOUT

struct fl_ui_layout {
  int win_width;
  int win_height;
  float win_widthf;
  float win_heightf;

  float toolbar_width;
  float depth_bar_height;
  float plot_height;

  float kinect_image_width;
  float kinect_image_height;

  float depth_bar_width;
};

void fl_ui_layout_calculate(fl_ui_layout* layout, int win_width,
                            int win_height);

#endif
