#include "Net.h"

namespace EngineCore {
	Server Server::self = Server();
	Packet(*Server::SendPacket) (Uint16 clientID);
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
		} config("./server.txt");

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

					std::vector<Uint16> acks = { packet->ack };

					Uint16 bitfield = packet->bitfield;
					for (Uint16 j = 0; j < 16; ++j)
						if ((bitfield & (1 << j)) >> j)
							acks.push_back(packet->ack - (j + 1));

					for (Uint16 j = 0; j < self.clients[i].sentPackets.size(); ++j) {
						auto it = std::find(acks.begin(), acks.end(), self.clients[i].sentPackets[j].second.packetID);
						if (it != acks.end()) {
							self.clients[i].sentPackets.erase(self.clients[i].sentPackets.begin() + j);
							j -= 1;
						}
					}

					{
						if (packet->packetID != NULL) {
							auto it = std::find(self.clients[i].to_ack.begin(), self.clients[i].to_ack.end(), packet->packetID);
							if (it != self.clients[i].to_ack.end()) {
								self.clients[i].lastReceiv = clock();
								delete packet;
							}
							else {
								self.clients[i].to_ack.push_back(packet->packetID);
								if (self.clients[i].to_ack.size() >= 17)
									self.clients[i].to_ack.erase(self.clients[i].to_ack.begin());
							}
						}
					}

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

				{
					if (packet->packetID != NULL) {
						auto it = std::find(self.clients[clientID].to_ack.begin(), self.clients[clientID].to_ack.end(), packet->packetID);
						if (it != self.clients[clientID].to_ack.end()) {
							self.clients[clientID].lastReceiv = clock();
							delete packet;
						}
						else {
							self.clients[clientID].to_ack.push_back(packet->packetID);
							if (self.clients[clientID].to_ack.size() >= 17)
								self.clients[clientID].to_ack.erase(self.clients[clientID].to_ack.begin());
						}
					}
				}

				if (Server::ConnectHandler)
					Server::ConnectHandler(clientID);
				if (Server::GetPacket)
					Server::GetPacket(clientID, packet);
			}

			delete packet;
		}

		for (Uint32 clientID = 0; clientID < self.clients.size(); ++clientID) {
			if (self.clients[clientID].ip.host == 0 || self.clients[clientID].ip.port == 0)
				continue;

			for (Uint16 i = 0; i < self.clients[clientID].sentPackets.size(); ++i)
				if (clock() > self.clients[clientID].sentPackets[i].first + 200) {
					self.clients[clientID].sentPackets[i].first = clock();
					self.clients[clientID].resend.push(self.clients[clientID].sentPackets[i].second);
				}

			if (clock() > self.clients[clientID].lastReceiv + 2000) {
				Log::info() << "Net. Processing disconnect (" << clientID << ")";

				if (Server::DisconnectHandler)
					Server::DisconnectHandler(clientID);

				self.clients[clientID].ip.host = self.clients[clientID].ip.port = 0;
			}
			else if (clock() > self.clients[clientID].lastSend + 50) {
				self.clients[clientID].lastSend = clock();

				if (!self.clients[clientID].to_ack.empty()) {
					self.clients[clientID].ack = *std::max_element(self.clients[clientID].to_ack.begin(), self.clients[clientID].to_ack.end());
					self.clients[clientID].bitfield = NULL;

					for (Uint8 i = 0; i < 16; ++i) {
						auto it = std::find(self.clients[clientID].to_ack.begin(), self.clients[clientID].to_ack.end(), self.clients[clientID].ack - i - 1);
						if (it != self.clients[clientID].to_ack.end())
							self.clients[clientID].bitfield = self.clients[clientID].bitfield | (1 << i);
					}
				}

				if (!self.clients[clientID].resend.empty()) {
					Server::send(clientID, self.clients[clientID].resend.front());
					self.clients[clientID].resend.pop();
				}
				else if (self.clients[clientID].queue.empty()) {
					Packet packet;

					if (Server::SendPacket)
						packet = Server::SendPacket(clientID);
					else packet = Packet::create(0);

					Server::send(clientID, packet);
				}
				else {
					self.clients[clientID].lastPacketID += 1;
					self.clients[clientID].queue.front().packetID = self.clients[clientID].lastPacketID;
					self.clients[clientID].sentPackets.push_back(std::make_pair(
						clock(),
						self.clients[clientID].queue.front()
					));

					Server::send(clientID, self.clients[clientID].queue.front());
					self.clients[clientID].queue.pop();
				}
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