#include "../Net.h"

namespace WindEngine {
	Client Client::self = Client();
	Packet(*Client::SendPacket) (void) = NULL;
	std::map<Uint16, void(*) (Packet*)> Client::codeHandlers = {};

	bool Client::isConnect() {
		return self.is_connect;
	}
	void Client::addToSend(Packet packet) {
		self.queue.push(packet);
	}
	void Client::addCodeHandler(Uint16 code, void(*handler) (Packet*)) {
		codeHandlers.insert(std::make_pair(code, handler));
	}

	void Client::send(Packet packet) {
		if (!self.is_connect) {
			Log::error() << "Couldn't send data because client is not connect to server";
			return;
		}

		packet.ack = self.ack;
		packet.bitfield = self.bitfield;

		memcpy(&packet.data[0], &packet.code, 2);
		memcpy(&packet.data[2], &packet.packetID, 2);
		memcpy(&packet.data[4], &packet.ack, 2);
		memcpy(&packet.data[6], &packet.bitfield, 2);

		static UDPpacket* udppacket = SDLNet_AllocPacket(packet.len);
		if (!udppacket) {
			Log::error() << "Client::send. Couldn't alloc UDP packet";
			return;
		}

		udppacket->address = self.server;
		udppacket->len = packet.len;

		if (packet.data)
			memcpy(udppacket->data, packet.data, packet.len);
		else {
			Log::error() << "Client::send. Packet data is null";
			return;
		}

		if (SDLNet_UDP_Send(self.socket, -1, udppacket) != 1)
			Log::error() << "SDLNet_UDP_Send:" << SDLNet_GetError();
	}
	Packet* Client::read() {
		static UDPpacket* packet = SDLNet_AllocPacket(256);

		if (!packet) {
			Log::error() << "Client::read. Couldn't alloc UDP packet";
			return NULL;
		}

		int result = SDLNet_UDP_Recv(self.socket, packet);

		if (result == -1) {
			Log::error() << SDLNet_GetError();
			return NULL;
		}

		if (result == 0)
			return NULL;

		return Packet::createFromUDP(packet);
	}
}