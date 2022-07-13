#include "Net.h"
#pragma comment(lib, "Ws2_32.lib")

namespace EngineCore {
	bool Net::isServer = true;

	bool Net::init() {
		Log::info() << "SDL net init";

		if (SDLNet_Init() == -1) {
			Log::error() << "SDLNet_GetError: " << SDLNet_GetError();
			return false;
		}

		if (isServer) {
			Server::start();
		} else {
			Client::connect();
		}

		return true;
	}

	void Net::update() {
		if (isServer)
			Server::update();
	}

	void Net::free() {
		Log::info() << "Free memory from SDLNet";

		if (isServer)
			Server::free();
		else
			Client::free();

		SDLNet_Quit();
	}

	void Net::send(UDPsocket socket, IPaddress* ip, byte* data, Uint32 len) {
		UDPpacket* packet = SDLNet_AllocPacket(len);
		
		if (!packet) {
			Log::error() << "Couldn't alloc UDP packet";
			return;
		}

		packet->address.host = ip->host;
		packet->address.port = ip->port;
		packet->len = len;
		
		memcpy(packet->data, data, len);

		if (
			SDLNet_UDP_Send(socket, -1, packet) == 0
		)
			Log::error() << "SDLNet_UDP_Send:" << SDLNet_GetError();
	}

	UDPpacket* Net::recieved(UDPsocket socket, SDLNet_SocketSet socket_set, Uint32 len) {
		UDPpacket* packet = SDLNet_AllocPacket(len);
		
		if (!packet) {
			Log::error() << "Couldn't alloc UDP packet";
			return NULL;
		}

		int num_ready = SDLNet_CheckSockets(socket_set, 1000);
		if (num_ready > 0) {
			int result = SDLNet_UDP_Recv(socket, packet);

			if (result == -1) {
				Log::error() << SDLNet_GetError();
				return NULL;
			}

			if (result == 0)
				return NULL;
			
			return packet;
		}

		Log::warning() << "Connection timed out";
		return NULL;
	}

	//---------------------------------------------------//

	Server Server::self = Server();

	void Server::start() {
		Log::info() << "Launched NetUDPServer...";

		struct _config {
			int port = 7708;
			int maxClients = 10;

			_config(std::string path) {
				Log::info() << "Read config NetUDPServer";

				Config config = ConfigReader::read(path);

				if (config.isVar("port")) port = config.getIntValue("port");
				if (config.isVar("maxClients")) maxClients = config.getIntValue("maxClients");
			}
		} config("asset/server.txt");

		Log::info() << "Launched NetUDPServer on port " << config.port;

		self.server_socket = SDLNet_UDP_Open(config.port);
		if (self.server_socket == NULL) {
			Log::error() << SDLNet_GetError();
			return;
		}

		self.socket_set = SDLNet_AllocSocketSet(config.maxClients + 1);
		if (self.socket_set == NULL) {
			Log::error() << SDLNet_GetError();
			return;
		}

		if (
			SDLNet_UDP_AddSocket(self.socket_set, self.server_socket) == -1
		) {
			Log::error() << SDLNet_GetError();
			return;
		}

		SDLNet_UDP_Unbind(self.server_socket, 0);

		self.run = true;
	}

	void Server::update() {
		if (!self.run) return;

		Uint32 num_ready = SDLNet_CheckSockets(self.socket_set, 0);

		if (num_ready > 0) {
			if (SDLNet_SocketReady(self.server_socket)) {
				UDPpacket* packet = Net::recieved(self.server_socket, self.socket_set, 6);
				if (!packet)
					return;

				Log::info() << "Processing new connection (" << self.clients.size() << ")";
			
				IPaddress ip;

				memcpy(&ip.port, &packet->data[0], 2);
				memcpy(&ip.host, &packet->data[2], 4);

				const char* data = "LifeIsBeutifulServer";
				Net::send(self.server_socket, &ip, (byte*)data, 21);
			}
		}
	}

	// ?
	void Server::close(UDPsocket socket) {
		if (
			SDLNet_UDP_DelSocket(self.socket_set, socket) == -1
		) {
			Log::error() << SDLNet_GetError();
			return;
		}
		SDLNet_UDP_Close(socket);
	}

	void Server::free() {
		if (!self.run) return;

		Log::info() << "Free memory from NetUDPServer";

		close(self.server_socket);

		SDLNet_FreeSocketSet(self.socket_set);
	}

	//---------------------------------------------------//

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

				if (config.isVar("serverIp")) ip = config.getIntValue("serverIp");
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

		byte data[6];
		memcpy(&data[0], &ip.port, 2);
		memcpy(&data[2], &ip.host, 4);

		Net::send(self.client_socket, &self.ip, data, 6);
		
		UDPpacket* package = Net::recieved(self.client_socket, self.socket_set, 21);
		if (!package) {
			Log::error() << "Couldn't connect to server";
			return;
		}

		Log::info() << package->data;

		self.run = true;
	}

	void Client::free() {
		if (!self.run) return;

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
}