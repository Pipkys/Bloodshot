#ifdef BS_NETWORKING_ON

#include "Networking.h"
#include "NetworkingSystem.h"

namespace Bloodshot::Private::Launch
{
	void FNetworkingSystem::Execute(float DeltaTime)
	{
		if (!Networking::IServer::IsRunning()) return;

		ENetEvent* const Event = Networking::IServer::GetEvent();

		while (Event)
		{
			BS_LOG(Trace, "Event!");

			ENetPacket* const Packet = Event->packet;
			const ENetPeer* const Peer = Event->peer;
			const ENetAddress& Address = Peer->address;
			const uint32_t Host = Address.host;
			const uint16_t Port = Address.port;

			switch (Event->type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					BS_LOG(Trace, "A new client connected from {}:{}", Host, Port);

					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					BS_LOG(Trace, "Client from {}:{} disconnected", Host, Port);

					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					BS_LOG(Trace, "Recieved packet of length {} containing {} from {} on channel {}",
						Packet->dataLength,
						(char*)Packet->data,
						(char*)Peer->data,
						Event->channelID);

					enet_packet_destroy(Packet);

					break;
				}
			}
		}
	}
}

#endif
