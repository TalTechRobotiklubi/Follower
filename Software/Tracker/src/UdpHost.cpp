#include "UdpHost.h"
#include "Peer.h"

#include <enet/enet.h>
#include <stdio.h>
#include <string.h>

struct UdpHost {
  ENetAddress address;
  ENetHost* host;
  ENetEvent event;
};

UdpHost* UdpHostCreate(const char* hostAddress, int port) {
  ENetAddress address;
  address.port = port;
  enet_address_set_host(&address, hostAddress);
  ENetHost* host = enet_host_create(&address, 8, 1, 0, 0);
  if (host == NULL) {
    return NULL;
  }

  UdpHost* udp = (UdpHost*)calloc(1, sizeof(UdpHost));
  udp->address = address;
  udp->host = host;

  return udp;
}

void UdpHostBroadcast(UdpHost* udp, const IoVec* buf) {
  ENetPacket* packet = enet_packet_create(buf->data, buf->len, 0);
  enet_host_broadcast(udp->host, 0, packet);
  enet_host_flush(udp->host);
}

const IoVec* UdpHostPoll(UdpHost* udp) {
  ENetEvent* event = &udp->event;

  if (event->type == ENET_EVENT_TYPE_RECEIVE) {
    enet_packet_destroy(event->packet);
  }

  bool gotEvent = enet_host_service(udp->host, event, 0) > 0;

  if (gotEvent) {
    switch (event->type) {
      case ENET_EVENT_TYPE_RECEIVE:
        printf("Got data %lu\n", event->packet->dataLength);
        break;
      default:
        printf("Enet unhandled\n");
        break;
    }
  }

  return NULL;
}

void UdpHostDestroy(UdpHost* udp) {
  enet_host_destroy(udp->host);
  free(udp);
}

void UdpHostAddPeer(UdpHost* udp, const Peer* peer) {
  ENetAddress addr;
  enet_address_set_host(&addr, peer->address);
  addr.port = peer->port;

  ENetPeer* remote = enet_host_connect(udp->host, &addr, 1, 0);

  if (!remote) {
    printf("Failed to init connection\n");
  }
}
