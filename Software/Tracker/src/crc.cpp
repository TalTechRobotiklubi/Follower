#include "crc.h"

uint16_t crc16_sum(const void* src, size_t length) {
  const uint8_t* byte_source = (uint8_t*)src;

  uint16_t sum = 0;
  for (size_t i = 0; i < length; i++) {
    sum += byte_source[i];
  }

  return sum;
}
