#include "Net.h"

namespace EngineCore {
	Client Client::self = Client();

	void Client::connect() {
		Log::info() << "Launched NetUDPClient...";

		struct _config {
			std::string ip = "127.0.0.1";
			int server_port = 7708;
			int client_port = 7706;

			_config(std::string path) {
				Log::info() << "Read config NetUDPClient";

				Config config = ConfigReader::read(path);

				if (config.isVar("serverIp")) ip = config.getStringValue("serverIp");
				if (config.isVar("serverPort")) server_port = config.getIntValue("serverPort");
				if (config.isVar("clientPort")) client_port = config.getIntValue("clientPort");
			}
		} config("asset/client.txt");

		Log::info() << "Launched NetUDPClient on port: " << config.client_port;

		self.client_socket = SDLNet_UDP_Open(config.client_port);
		if (self.client_socket == NULL) {
			Log::error() << SDLNet_GetError();
			return;
		}

		Log::info() << "Connect to NetUDPSerever on address " << config.ip << ":" << config.server_port;

		if (SDLNet_ResolveHost(&self.ip, config.ip.c_str(), config.server_port) == -1) {
			Log::error() << SDLNet_GetError();
			return;
		}

		self.socket_set = SDLNet_AllocSocketSet(1);
		if (self.socket_set == NULL) {
			Log::error() << SDLNet_GetError();
			return;
		}

		if (SDLNet_UDP_AddSocket(self.socket_set, self.client_socket) == -1) {
			Log::error() << SDLNet_GetError();
			return;
		}

		if (SDLNet_UDP_Bind(self.client_socket, 0, &self.ip) == -1) {
			Log::error() << SDLNet_GetError();
			return;
		}

		char hostname[128];
		if (gethostname(hostname, 128) < 0)
			Log::error() << "Couldn't get local hostname";
		Log::info() << "hostname: " << hostname;

		IPaddress ip;
		if (SDLNet_ResolveHost(&ip, hostname, config.client_port) == -1) {
			Log::error() << SDLNet_GetError();
			return;
		}

		self.run = true;

		Packet packet = Packet(6);
		packet.write<Uint16>(ip.port);
		packet.write<Uint32>(ip.host);

		Log::info() << "SEND DATA " << ip.host << ":" << ip.port;

		Client::Send(packet);
	}

	void Client::update() {
		if (!self.run) return;

		Uint32 num_ready = SDLNet_CheckSockets(self.socket_set, 0);

		if (num_ready > 0) {
			if (SDLNet_SocketReady(self.client_socket)) {
				UDPpacket* package = Net::recieved(self.client_socket, 128);
				if (!package)
					return;

				Log::info() << "RESPONSE";

				SDLNet_FreePacket(package);
			}
		}
	}

	void Client::disconnect() {
		if (!self.run) return;

		Log::info() << "Disconnect...";

	}

	void Client::free() {
		if (!self.run) return;

		disconnect();

		Log::info() << "Free memory from NetUDPClient";

		if (
			SDLNet_UDP_DelSocket(self.socket_set, self.client_socket) == -1
			) {
			Log::error() << SDLNet_GetError();
			return;
		}
		SDLNet_UDP_Close(self.client_socket);

		SDLNet_FreeSocketSet(self.socket_set);
	}

	void Client::Send(Packet packet) {
		if (!self.run) {
			Log::warning() << "Failed to send data because there is no connection to the server";
			return;
		}

		packet.c_data = self.id;

		memcpy(&packet.data[0], &packet.code, 2);
		memcpy(&packet.data[2], &packet.packetID, 2);
		memcpy(&packet.data[4], &packet.c_data, 2);

		Net::send(self.client_socket, &self.ip, packet.data, packet.len);
	}
}