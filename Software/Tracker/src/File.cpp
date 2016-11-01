#include "File.h"
#include <stdio.h>

IoVec LoadFile(const char* name) {
  IoVec out;
  FILE* f = fopen(name, "rb");
  if (!f) {
    return out;
  }

  fseek(f, 0, SEEK_END);
  int64_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  out.data = (uint8_t*)calloc(size, 1);
  out.len = size;
  fread(out.data, 1, size, f);
  fclose(f);

  return out;
}

bool SaveFile(const char* name, const uint8_t* data, size_t len) {
  FILE* f = fopen(name, "wb");
  
  if (!f) return false;

  size_t written = fwrite(data, 1, len, f);
  fclose(f);

  return written == len;
}
