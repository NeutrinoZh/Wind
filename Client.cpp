#include "Net.h"

namespace EngineCore {
	Client Client::self = Client();
	Packet(*Client::SendPacket) (void) = NULL;
	void (*Client::GetPacket) (Packet* packet) = NULL;

	void Client::connect() {
		Log::info() << "Launched NetUDPClient";

		struct _config {
			std::string ip = "127.0.0.1";
			int port = 7708;

			_config(std::string path) {
				Log::info() << "Read config NetUDPClient: " << path;

				Config config = ConfigReader::read(path);

				if (config.isVar("ip")) ip = config.getStringValue("ip");
				if (config.isVar("port")) port = config.getIntValue("port");
			}
		} config("./connect.txt");

		self.socket = SDLNet_UDP_Open(0);
		if (self.socket == NULL) {
			Log::error() << SDLNet_GetError();
			return;
		}

		Log::info() << "Connect to NetUDPSerever on address " << config.ip << ":" << config.port;

		if (SDLNet_ResolveHost(&self.server, config.ip.c_str(), config.port) == -1) {
			Log::error() << SDLNet_GetError();
			return;
		}

		self.lastReceiv = clock();
		self.is_connect = true;
	}

	void Client::update() {
		if (!self.is_connect) return;

		while (true) {
			Packet* packet = Client::read();
			if (!packet)
				break;

			if (packet->packetID != NULL) {
				auto it = std::find(self.to_ack.begin(), self.to_ack.end(), packet->packetID);
				if (it != self.to_ack.end()) {
					self.lastReceiv = clock();
					delete packet;
					continue;
				}
				else {
					self.to_ack.push_back(packet->packetID);
					if (self.to_ack.size() >= 17)
						self.to_ack.erase(self.to_ack.begin());
				}
			}

			std::vector<Uint16> acks = { packet->ack };

			Uint16 bitfield = packet->bitfield;
			for (Uint16 j = 0; j < 16; ++j)
				if ((bitfield & (1 << j)) >> j)
					acks.push_back(packet->ack - (j + 1));

			for (Uint16 j = 0; j < self.sentPackets.size(); ++j) {
				auto it = std::find(acks.begin(), acks.end(), self.sentPackets[j].second.packetID);
				if (it != acks.end()) {
					self.sentPackets.erase(self.sentPackets.begin() + j);
					j -= 1;
				}
			}

			if (packet->len == 256) {
				Uint8 i = packet->read<Uint8>(),
					len = packet->read<Uint8>();

				if (!self.big_data.len) {
					self.big_data = Packet::create(246 * len);
					self.big_data.code = 0;
					self.big_data.address = packet->address;
				}

				self.big_data.code += 1;

				memcpy(&self.big_data.data[i * 246], &packet->data[10], packet->len - 10);

				if (self.big_data.code == len) {
					self.big_data.code = packet->code;
					if (Client::GetPacket)
						Client::GetPacket(&self.big_data);
					self.big_data.len = NULL;
				}

			}
			else if (Client::GetPacket)
				Client::GetPacket(packet);

			self.lastReceiv = clock();
			delete packet;
		}

		if (clock() > self.lastReceiv + 10000) {
			Log::info() << "Server not responding";
			self.disconnect();
		}
		else if (clock() > self.lastSend + 50) {
			self.lastSend = clock();

			for (Uint16 i = 0; i < self.sentPackets.size(); ++i)
				if (clock() > self.sentPackets[i].first + 200) {
					self.sentPackets[i].first = clock();
					self.resend.push(self.sentPackets[i].second);
				}

			if (!self.to_ack.empty()) {
				self.ack = *std::max_element(self.to_ack.begin(), self.to_ack.end());
				self.bitfield = NULL;

				for (Uint8 i = 0; i < 16; ++i) {
					auto it = std::find(self.to_ack.begin(), self.to_ack.end(), self.ack - i - 1);
					if (it != self.to_ack.end()) {
						self.bitfield = self.bitfield | (1 << i);
					}
				}
			}

			if (!self.resend.empty()) {
				Client::send(self.resend.front());
				self.resend.pop();
			}
			else if (self.queue.empty()) {
				Packet packet;

				if (Client::SendPacket)
					packet = Client::SendPacket();
				else packet = Packet::create(0);

				Client::send(packet);
			}
			else {
				self.lastPacketID += 1;
				self.queue.front().packetID = self.lastPacketID;
				self.sentPackets.push_back(std::make_pair(
					clock(),
					self.queue.front()
				));

				Client::send(self.queue.front());
				self.queue.pop();
			}
		}
	}

	void Client::disconnect() {
		if (!self.is_connect) return;
		self.is_connect = false;

		Log::info() << "NetUDPClient disconnect";

		SDLNet_UDP_Close(self.socket);
	}

	void Client::addToSend(Packet packet) {
		self.queue.push(packet);
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