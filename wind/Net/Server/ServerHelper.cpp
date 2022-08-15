#include "../Net.h"

namespace WindEngine {
	Server Server::self = Server();
	Packet(*Server::SendPacket) (Uint16 clientID);
	std::map<Uint16, void(*) (Uint16, Packet*)> Server::codeHandlers = {};

	void(*Server::ConnectHandler) (Uint16 clientID);
	void(*Server::DisconnectHandler) (Uint16 clientID);

	void Server::addCodeHandler(Uint16 code, void(*handler) (Uint16, Packet*)) {
		Server::codeHandlers.insert(std::make_pair(code, handler));
	}

	void Server::addToSend(Uint16 clientID, Packet packet) {
		if (clientID >= self.clients.size()) {
			Log::error() << "Couldn't add packet to queue send because index out of clients array size";
			return;
		}

		self.clients[clientID].queue.push(packet);
	}
	void Server::addToSendToEveryone(Packet packet) {
		for (Uint16 i = 0; i < self.clients.size(); ++i)
			if (self.clients[i].ip.host == 0 || self.clients[i].ip.port == 0)
				continue;
			else
				addToSend(i, packet);
	}
	void Server::addToSendData(Uint16 clientID, Packet data) {
		Uint8 num_packets = (Uint8)std::ceil(data.len / 246.0);
		for (Uint8 i = 0; i < num_packets; ++i) {
			Packet packet = Packet::create(248);
			packet.code = data.code;

			memcpy(&packet.data[8], &i, 1);
			memcpy(&packet.data[9], &num_packets, 1);
			memcpy(&packet.data[10], &data.data[i * 246], 246);

			addToSend(clientID, packet);
		}
	}

	void Server::send(Uint16 clientID, Packet packet) {
		if (!self.is_run) {
			Log::error() << "Couldn't send data because client is not connect to server";
			return;
		}

		packet.ack = self.clients[clientID].ack;
		packet.bitfield = self.clients[clientID].bitfield;

		memcpy(&packet.data[0], &packet.code, 2);
		memcpy(&packet.data[2], &packet.packetID, 2);
		memcpy(&packet.data[4], &packet.ack, 2);
		memcpy(&packet.data[6], &packet.bitfield, 2);

		static UDPpacket* udppacket = SDLNet_AllocPacket(packet.len);
		if (!udppacket) {
			Log::error() << "Client::send. Couldn't alloc UDP packet";
			return;
		}

		udppacket->address = self.clients[clientID].ip;
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
	Packet* Server::read() {
		static UDPpacket* packet = SDLNet_AllocPacket(256);

		if (!packet) {
			Log::error() << "Couldn't alloc UDP packet";
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