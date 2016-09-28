#include "core_opt.h"
#include "core.h"
#include "parg/parg.h"
#include <stdio.h>
#include "fl_sqlite_source.h"
#include "kinect_null_frame_source.h"
#include "kinect_live_source.h"

int parse_opt(core* c, int argc, char** argv) {
  parg_state args;
  parg_init(&args);

  int res = -1;
  while ((res = parg_getopt(&args, argc, argv, "d:h")) != -1) {
    switch (res) {
      case 'd': 
        c->frame_source = new fl_sqlite_source(args.optarg);
        printf("frame source: database\n");
        break;
      default: break;
    }
  }

  if (!c->frame_source) {
#ifdef FL_KINECT_ENABLED
    printf("frame source: kinect\n");
    c->frame_source = new kinect_live_source();
#else
    printf("frame source: null\n");
    c->frame_source = new kinect_null_frame_source();
#endif
  }

  return 1;
}
