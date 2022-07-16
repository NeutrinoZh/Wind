#include "Net.h"

namespace EngineCore {
	Server Server::self = Server();
	void (*Server::RequestHandler) (Uint16 code, Uint32 ID, byte* data, Uint32 len) = NULL;
	void (*Server::ConnectHandler) (Uint32 id) = NULL;
	void (*Server::DisconnectHandler) (Uint32 id) = NULL;

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

					if (DisconnectHandler)
						DisconnectHandler(id);

					self.clients[id].host = self.clients[id].port = 0;
				} else if (code == CONNECT) {
					Uint32 id = getID();

					IPaddress ip;

					memcpy(&ip.port, &packet->data[2], 2);
					memcpy(&ip.host, &packet->data[4], 4);

					Log::info() << "Net. Processing new connection (" << id << ") " << ip.host << ":" << ip.port;

					for (IPaddress i : self.clients)
						if (ip.host == i.host && ip.port == i.port) {
							Log::info() << "The client is already connected, so will ignore the request.";
							return;
						}

					byte data[4];
					memcpy(&data[0], &id, sizeof(Uint32));

					Net::send(self.server_socket, &ip, (byte*)data, sizeof(Uint32));

					self.clients[id] = ip;

					if (ConnectHandler)
						ConnectHandler(id);
				} else {
					Uint32 ID;
					memcpy(&ID, &packet->data[2], sizeof(Uint32));

					if (RequestHandler)
						RequestHandler(code, ID, packet->data, packet->len);
					else
						Log::warning() << "Net. Missing castom request handler; Code:" << code;
				}

				SDLNet_FreePacket(packet);
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

	std::vector<IPaddress> Server::getClients() {
		return self.clients;
	}

	void Server::Send(Uint32 id, Uint16 code, byte* data, Uint32 len) {
		if (!self.run) {
			Log::warning() << "Failed to send data because the server was not started";
			return;
		}

		memcpy(&data[0], &code, 2);

		Net::send(self.server_socket, &self.clients[id], (byte*)data, len);
	}
}