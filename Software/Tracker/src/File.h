#pragma once
#include "IoVec.h"

IoVec LoadFile(const char* name);
bool SaveFile(const char* name, const uint8_t* data, size_t len);
