#include "core_opt.h"
#include <fhd.h>
#include <fhd_classifier.h>
#include <stdio.h>
#include "ScriptLoader.h"
#include "UdpHost.h"
#include "core.h"
#include "fl_constants.h"
#include "fl_sqlite_source.h"
#include "kinect_live_source.h"
#include "kinect_null_frame_source.h"
#include "parg/parg.h"

int parse_opt(core* c, int argc, char** argv) {
  parg_state args;
  parg_init(&args);

  int res = -1;
  const char* host = "127.0.0.1";
  uint16_t port = 9001;
  while ((res = parg_getopt(&args, argc, argv, "i:c:d:h:p:s:")) != -1) {
    switch (res) {
      case 'd':
        c->frame_source = new fl_sqlite_source(args.optarg);
        printf("frame source: database\n");
        break;
      case 's':
        c->serial.start(args.optarg, 115200);
        printf("serial %s\n", args.optarg);
        break;
      case 'c': {
        fhd_classifier* classifier = fhd_classifier_create(args.optarg);
        if (classifier) {
          c->fhd = (fhd_context*)calloc(1, sizeof(fhd_context));
          fhd_context_init(c->fhd, kDepthWidth, kDeptHeight, 8, 8);
          c->fhd->classifier = classifier;
          printf("using classifier - %s\n", args.optarg);
        } else {
          printf("could not open classifier: %s\n", args.optarg);
        }
        break;
      }
      case 'h': {
        host = args.optarg;
        break;
      }
      case 'p': {
        port = atoi(args.optarg);
        break;
      }
      case 'i': {
        printf("initial script file: %s\n", args.optarg);
        if (!ScriptLoaderExecFile(&c->scripts, args.optarg)) {
          printf("failed to load %s\n", args.optarg);
        }
        break;
      }
      default:
        break;
    }
  }

  c->udp = UdpHostCreate(host, port);

  if (!c->udp) {
    fprintf(stderr, "Failed to create UDP host.\n");
    abort();
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

  printf("host %s:%u\n", host, port);

  return 1;
}
