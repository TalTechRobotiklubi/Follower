#pragma once

#include "IoVec.h"
#include <string>

struct UdpHost;
struct Peer;

struct NetAddress {
  const char* host = nullptr;
  uint16_t port = 0;
};

UdpHost* UdpHostCreate(const char* hostAddress, int port);
void UdpHostBroadcast(UdpHost* udp, const uint8_t* data, int len);
const IoVec* UdpHostPoll(UdpHost* udp);
void UdpHostDestroy(UdpHost* udp);
void UdpHostAddPeer(UdpHost* udp, const Peer* peer);
