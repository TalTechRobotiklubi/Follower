#pragma once

#include <stdint.h>

struct Peer {
  char address[16];
  uint16_t port;
  uint8_t connected;
};

