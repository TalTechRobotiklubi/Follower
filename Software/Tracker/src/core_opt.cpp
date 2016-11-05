#include "core_opt.h"
#include <fhd.h>
#include <fhd_classifier.h>
#include <stdio.h>
#include "ScriptLoader.h"
#include "UdpHost.h"
#include "core.h"
#include "fl_constants.h"
#include "KinectLiveFrameSource.h"
#include "KinectNullFrameSource.h"
#include "KinectSqliteFrameSource.h"
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
        c->frameSource = new KinectSqliteFrameSource(args.optarg);
        printf("frame source: database\n");
        break;
      case 's':
        c->serial.start(args.optarg, 115200);
        printf("serial %s\n", args.optarg);
        break;
      case 'c': {
        fhd_classifier* classifier = fhd_classifier_create(args.optarg);
        if (classifier) {
          c->classifier = classifier;
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

  if (!c->frameSource) {
#ifdef FL_KINECT_ENABLED
    printf("frame source: kinect\n");
    c->frameSource = new KinectLiveFrameSource();
#else
    printf("frame source: null\n");
    c->frameSource = new KinectNullFrameSource();
#endif
  }

  printf("host %s:%u\n", host, port);

  return 1;
}
