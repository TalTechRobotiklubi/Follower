#include "UdpHost.h"

#include <enet/enet.h>
#include <stdio.h>
#include <string.h>
#include <mutex>

std::once_flag enetInit;

const size_t kInputBufferSize = 8092;

struct UdpHost {
  ENetAddress address;
  ENetHost* host;
  size_t inputBufferSize;
  IoVec input;
};

UdpHost* UdpHostCreate(const char* hostAddress, int port) {
  std::call_once(enetInit, enet_initialize);
  ENetAddress address;
  if (hostAddress) {
    enet_address_set_host(&address, hostAddress);
  } else {
    address.host = ENET_HOST_ANY;
  }
  address.port = port;
  ENetHost* host = enet_host_create(&address, 8, 1, 0, 0);
  if (host == NULL) {
    return NULL;
  }

  UdpHost* udp = (UdpHost*)calloc(1, sizeof(UdpHost));
  udp->address = address;
  udp->host = host;
  udp->inputBufferSize = kInputBufferSize;
  udp->input.data = (uint8_t*)calloc(kInputBufferSize, 1);
  udp->input.len = 0;

  return udp;
}

void UdpHostBroadcast(UdpHost* udp, const uint8_t* data, int len,
                      bool reliable) {
  ENetPacket* packet =
      enet_packet_create(data, len, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
  enet_host_broadcast(udp->host, 0, packet);
  enet_host_flush(udp->host);
}

const IoVec* UdpHostPoll(UdpHost* udp) {
  ENetEvent event;

  if (enet_host_service(udp->host, &event, 0) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_RECEIVE: {
        ENetPacket* packet = event.packet;
        if (packet->dataLength > udp->inputBufferSize) {
          free(udp->input.data);
          udp->inputBufferSize = size_t(1.5 * packet->dataLength);
          udp->input.data = (uint8_t*)calloc(udp->inputBufferSize, 1);
        }
        memcpy(udp->input.data, packet->data, packet->dataLength);
        udp->input.len = int(packet->dataLength);
        enet_packet_destroy(event.packet);
        return &udp->input;
      }
      default:
        break;
    }
  }

  return nullptr;
}

void UdpHostDestroy(UdpHost* udp) {
  enet_host_destroy(udp->host);
  free(udp);
}
