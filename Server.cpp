#include "Net.h"

namespace EngineCore {
	Server Server::self = Server();
	Packet(*Server::SendPacket) (Uint16 clientID, Uint16 packetID);
	void(*Server::GetPacket) (Uint16 clientID, Packet* packet);

	void(*Server::ConnectHandler) (Uint16 clientID);
	void(*Server::DisconnectHandler) (Uint16 clientID);

	void Server::run() {
		Log::info() << "Launched NetUDPServer";

		struct _config {
			int port = 7708;

			_config(std::string path) {
				Log::info() << "Read config NetUDPServer: " << path;

				Config config = ConfigReader::read(path);

				if (config.isVar("port")) port = config.getIntValue("port");
			}
		} config("./asset/server.txt");

		self.socket = SDLNet_UDP_Open(config.port);
		if (self.socket == NULL) {
			Log::error() << "SDLNet_UDP_Open: " << SDLNet_GetError();
			return;
		}

		self.is_run = true;
	}

	void Server::update() {
		if (!self.is_run) return;

		while (true) {
			Packet* packet = Server::read();
			if (!packet)
				break;

			bool newClient = true;
			for (Uint32 i = 0; i < self.clients.size(); ++i)
				if (
					self.clients[i].ip.host == packet->address.host &&
					self.clients[i].ip.port == packet->address.port
					) {
					newClient = false;

					if (Server::GetPacket)
						Server::GetPacket(i, packet);

					self.clients[i].lastReceiv = clock();

					break;
				}

			if (newClient) {
				Uint32 clientID = getID();

				Log::info() << "Server. Proccesing new connection (" << clientID << ") " <<
					packet->address.host << ":" << packet->address.port;

				self.clients[clientID].ip = packet->address;
				self.clients[clientID].lastReceiv = clock();

				if (Server::ConnectHandler)
					Server::ConnectHandler(clientID);
				if (Server::GetPacket)
					Server::GetPacket(clientID, packet);
			}

			delete packet;
		}

		for (Uint32 clientID = 0; clientID < self.clients.size(); ++clientID)
			if (self.clients[clientID].ip.host == 0 || self.clients[clientID].ip.port == 0)
				continue;
			else if (clock() > self.clients[clientID].lastReceiv + 3000) {
				Log::info() << "Net. Processing disconnect (" << clientID << ")";

				if (Server::DisconnectHandler)
					Server::DisconnectHandler(clientID);

				self.clients[clientID].ip.host = self.clients[clientID].ip.port = 0;
			} else if (clock() > self.clients[clientID].lastSend + 50) {
				self.clients[clientID].lastPacketID += 1;
				self.clients[clientID].lastSend = clock();

				if (self.clients[clientID].queue.empty()) {
					Packet packet;

					if (Server::SendPacket)
						packet = Server::SendPacket(clientID, self.clients[clientID].lastPacketID);
					else packet = Packet::create(0);

					Server::send(clientID, packet);
				} else {
					Server::send(clientID, self.clients[clientID].queue.front());
					self.clients[clientID].queue.pop();
				}
			}
	}

	void Server::shutdown() {
		if (!self.is_run) return;

		Log::info() << "Shutdown NetUDPServer";

		SDLNet_UDP_Close(self.socket);
	}

	//==============================================================//

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

	void Server::send(Uint16 clientID, Packet packet) {
		if (!self.is_run) {
			Log::error() << "Couldn't send data because client is not connect to server";
			return;
		}

		packet.packetID = self.clients[clientID].lastPacketID;

		memcpy(&packet.data[0], &packet.code, 2);
		memcpy(&packet.data[2], &packet.packetID, 2);

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

	Uint16 Server::getID() {
		for (Uint32 i = 0; i < self.clients.size(); ++i)
			if (
				self.clients[i].ip.host == 0 &&
				self.clients[i].ip.port == 0
				) return i;

		self.clients.push_back(Client());
		return (Uint16)(self.clients.size() - 1);
	}
}