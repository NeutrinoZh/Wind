#include "Net.h"
#pragma comment(lib, "Ws2_32.lib")

namespace EngineCore {
	bool Net::isServer = false;

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
		else Client::update();
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
		if (ip->host == 0 && ip->port == 0) {
			Log::error() << "Couldn't send data to NULL ip";
			return;
		}

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

	UDPpacket* Net::recieved(UDPsocket socket, Uint32 len) {
		UDPpacket* packet = SDLNet_AllocPacket(len);
		
		if (!packet) {
			Log::error() << "Couldn't alloc UDP packet";
			return NULL;
		}

		int result = SDLNet_UDP_Recv(socket, packet);

		if (result == -1) {
			Log::error() << SDLNet_GetError();
			return NULL;
		}

		if (result == 0)
			return NULL;
			
		return packet;
	}

	//---------------------------------------------------//

	Server Server::self = Server();
	void (*Server::RequestHandler) (Uint16 code, byte* data, Uint32 len) = NULL;
	void (*Server::ConnectHandler) (Uint32 id) = NULL;

	namespace {
		const Uint32 DISCONNECT = 9797;
		const Uint32 CONNECT = 9798;
	};

	void Server::start() {
		Log::info() << "Launched NetUDPServer...";

		struct _config {
			int port = 7708;

			_config(std::string path) {
				Log::info() << "Read config NetUDPServer";

				Config config = ConfigReader::read(path);

				if (config.isVar("port")) port = config.getIntValue("port");
			}
		} config("asset/server.txt");

		Log::info() << "Launched NetUDPServer on port " << config.port;

		self.server_socket = SDLNet_UDP_Open(config.port);
		if (self.server_socket == NULL) {
			Log::error() << SDLNet_GetError();
			return;
		}

		self.socket_set = SDLNet_AllocSocketSet(1);
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

	Uint32 Server::getID() {
		for (Uint32 i = 0; i < self.clients.size(); ++i)
			if (
			  self.clients[i].host == 0 &&
			  self.clients[i].port == 0
			) return i;
		
		self.clients.push_back(IPaddress());
		return self.clients.size() - 1;
	}

	void Server::update() {
		if (!self.run) return;

		Uint32 num_ready = SDLNet_CheckSockets(self.socket_set, 0);

		if (num_ready > 0) {
			if (SDLNet_SocketReady(self.server_socket)) {
				UDPpacket* packet = Net::recieved(self.server_socket, 128);
				if (!packet) {
					return;
				}

				Uint16 code;
				memcpy(&code, &packet->data[0], sizeof(Uint16));
				if (code == DISCONNECT) {
					Uint32 id;
					memcpy(&id, &packet->data[2], sizeof(Uint32));

					Log::info() << "Net. Processing disconnect (" << id << ")";

					self.clients[id].host = self.clients[id].port = 0;
				} else if (code == CONNECT) {
					Uint32 id = getID();
					Log::info() << "Net. Processing new connection (" << id << ")";

					IPaddress ip;

					memcpy(&ip.port, &packet->data[2], 2);
					memcpy(&ip.host, &packet->data[4], 4);

					byte data[4];
					memcpy(&data[0], &id, sizeof(Uint32));

					Net::send(self.server_socket, &ip, (byte*)data, sizeof(Uint32));

					self.clients[id] = ip;

					SDLNet_FreePacket(packet);

					if (ConnectHandler)
						ConnectHandler(id);
				} else {
					if (RequestHandler)
						RequestHandler(code, packet->data, packet->len);
					else
						Log::warning() << "Net. Missing castom request handler; Code:" << code;
				}
			}
		}
	}

	void Server::free() {
		if (!self.run) return;

		Log::info() << "Free memory from NetUDPServer";

		if (
			SDLNet_UDP_DelSocket(self.socket_set, self.server_socket) == -1
		) {
			Log::error() << SDLNet_GetError();
			return;
		}
		SDLNet_UDP_Close(self.server_socket);

		SDLNet_FreeSocketSet(self.socket_set);
	}


	void Server::Send(Uint32 id, byte* data, Uint32 len) {
		if (!self.run) {
			Log::warning() << "Failed to send data because the server was not started";
			return;
		}

		Net::send(self.server_socket, &self.clients[id], (byte*)data, len);
	}


	//---------------------------------------------------//

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

		byte data[8];
		memcpy(&data[0], &CONNECT, 2);
		memcpy(&data[2], &ip.port, 2);
		memcpy(&data[4], &ip.host, 4);

		Net::send(self.client_socket, &self.ip, data, 8);
		
		Uint32 num_ready = SDLNet_CheckSockets(self.socket_set, 1000);
		if (num_ready > 0) {
			UDPpacket* package = Net::recieved(self.client_socket, sizeof(Uint32));
			if (!package) {
				Log::error() << "Couldn't connect to server";
				return;
			}

			memcpy(&self.id, &package->data[0], sizeof(Uint32));
			self.run = true;

			Log::info() << "Connected with ID:" << self.id;
		} else {
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