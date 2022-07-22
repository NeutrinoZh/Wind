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
		} config("./asset/connect.txt");

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

		self.is_connect = true;
	}

	void Client::update() {
		if (!self.is_connect) return;

		while (true) {
			Packet* packet = Client::read();
			if (!packet)
				break;

			if (Client::GetPacket)
				Client::GetPacket(packet);

			self.lastReceiv = clock();
			delete packet;
		}

		if (clock() > self.lastReceiv + 3000) {
			Log::info() << "Server not responding";
			self.disconnect();
		}
		else if (clock() > self.lastSend + 50) {
			self.lastSend = clock();
			self.lastPacketID += 1;

			if (self.queue.empty()) {
				Packet packet;

				if (Client::SendPacket)
					packet = Client::SendPacket();
				else packet = Packet::create(0);

				Client::send(packet);
			}
			else {
				Client::send(self.queue.back());
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

		memcpy(&packet.data[0], &packet.code, 2);
		memcpy(&packet.data[2], &packet.packetID, 2);

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