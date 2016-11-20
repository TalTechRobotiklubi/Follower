#pragma once

#include <string>
#include "IoVec.h"

struct UdpHost;

struct NetAddress {
  const char* host = nullptr;
  uint16_t port = 0;
};

UdpHost* UdpHostCreate(const char* hostAddress, int port);
void UdpHostBroadcast(UdpHost* udp, const uint8_t* data, int len,
                      bool reliable);
const IoVec* UdpHostPoll(UdpHost* udp);
void UdpHostDestroy(UdpHost* udp);
