#pragma once

#include "IoVec.h"

struct UdpHost;
struct Peer;

UdpHost* UdpHostCreate(const char* hostAddress, int port);
void UdpHostBroadcast(UdpHost* udp, const uint8_t* data, int len);
const IoVec* UdpHostPoll(UdpHost* udp);
void UdpHostDestroy(UdpHost* udp);
void UdpHostAddPeer(UdpHost* udp, const Peer* peer);
