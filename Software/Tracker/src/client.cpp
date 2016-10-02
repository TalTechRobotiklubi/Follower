#include <assert.h>
#include <enet/enet.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char** argv) {
  ENetHost* client = enet_host_create(nullptr, 1, 1, 0, 0);

  if (!client) {
    printf("An error occurred while trying to create an ENet client host.\n");
    exit(EXIT_FAILURE);
  }

  ENetAddress address;
  enet_address_set_host(&address, "127.0.0.1");
  address.port = 9001;

  ENetPeer* peer = enet_host_connect(client, &address, 1, 0);

  if (!peer) {
    printf("failed to initialize connection\n");
    exit(EXIT_FAILURE);
  }

	for (;;) {
  	ENetEvent event;
  	if (enet_host_service(client, &event, 0) > 0) {
			switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
          printf("connected\n");
          break;
        case ENET_EVENT_TYPE_DISCONNECT:
          printf("disconnect\n");
          break;
				case ENET_EVENT_TYPE_RECEIVE:
					printf("Got data %lu\n", event.packet->dataLength);
					break;
				default:
					printf("Enet unhandled\n");
					break;
			}
		}

	}

  enet_host_destroy(client);

  return 0;
}
