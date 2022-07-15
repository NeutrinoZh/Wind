#include "Net.h"

namespace EngineCore {
	Client Client::self = Client();
	void (*Client::ResponseHandler) (byte* data, Uint32 len) = NULL;

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

		byte data[8];
		memcpy(&data[0], &CONNECT, 2);
		memcpy(&data[2], &ip.port, 2);
		memcpy(&data[4], &ip.host, 4);

		Uint32 num_ready;

		for (Uint32 i = 0; i < 100; ++i) {
			Net::send(self.client_socket, &self.ip, data, 8);
			num_ready = SDLNet_CheckSockets(self.socket_set, 10);
			if (num_ready > 0)
				break;
		}

		if (num_ready > 0) {
			UDPpacket* package = Net::recieved(self.client_socket, sizeof(Uint32));
			if (!package) {
				Log::error() << "Couldn't connect to server";
				return;
			}

			memcpy(&self.id, &package->data[0], sizeof(Uint32));
			self.run = true;

			Log::info() << "Connected with ID:" << self.id;
		}
		else {
			Log::warning() << "Connection timeout";
		}
	}

	void Client::update() {
		if (!self.run) return;

		Uint32 num_ready = SDLNet_CheckSockets(self.socket_set, 0);

		if (num_ready > 0) {
			if (SDLNet_SocketReady(self.client_socket)) {
				UDPpacket* package = Net::recieved(self.client_socket, 128);
				if (!package)
					return;

				if (ResponseHandler)
					ResponseHandler(package->data, package->len);
				else Log::warning() << "Net. Missing ResponseHandler";

				SDLNet_FreePacket(package);
			}
		}
	}

	void Client::disconnect() {
		if (!self.run) return;

		Log::info() << "Disconnect...";

		byte data[6];
		memcpy(&data[0], &DISCONNECT, 2);
		memcpy(&data[2], &self.id, 4);

		Net::send(self.client_socket, &self.ip, data, 6);
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

	void Client::Send(byte* data, Uint32 len) {
		if (!self.run) {
			Log::warning() << "Failed to send data because there is no connection to the server";
			return;
		}

		Net::send(self.client_socket, &self.ip, data, len);
	}
}