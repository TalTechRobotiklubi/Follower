#define ZMQ_BUILD_DRAFT_API
#include <zmq.h>
#include <string.h>
#include <assert.h>

int main(int argc, char** argv) {
  void* zmq = zmq_ctx_new();
  void* radio = zmq_socket(zmq, ZMQ_RADIO);
  int rc = zmq_connect(radio, "udp://127.0.0.1:7129");
  assert(rc == 0);

  for (;;) {
    char buf[10];
    memset(buf, 0, 10);

    zmq_msg_t msg;
    rc = zmq_msg_init(&msg);
    int bytes = zmq_msg_recv(&msg, radio, 0);
    if (bytes == -1) {
      printf("%s\n", zmq_strerror(zmq_errno()));
      zmq_msg_close(&msg);
    }
  }

  zmq_close(radio);
  zmq_ctx_destroy(zmq);
  return 0;
}
