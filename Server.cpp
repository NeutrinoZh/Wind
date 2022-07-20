#include "Net.h"

namespace EngineCore {
	Server Server::self = Server();
	void (*Server::ConnectHandler)	  (Uint32 id) = NULL;
	void (*Server::DisconnectHandler) (Uint32 id) = NULL;
	void (*Server::RequestHandler)    (Uint32 id, Packet* packet) = NULL;

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

	Uint16 Server::getID() {
		for (Uint32 i = 1; i < self.clients.size(); ++i)
			if (
				self.clients[i].ip.host == 0 &&
				self.clients[i].ip.port == 0
				) return i;

		self.clients.push_back(Client());
		return (Uint16)(self.clients.size() - 1);
	}

	void Server::update() {
		if (!self.run) return;

		Uint32 num_ready = SDLNet_CheckSockets(self.socket_set, 0);

		if (num_ready > 0) {
			if (SDLNet_SocketReady(self.server_socket)) {
				UDPpacket* udp_packet = Net::recieved(self.server_socket, 128);
				if (!udp_packet) {
					return;
				}

				Packet packet = Packet(udp_packet->data, udp_packet->len);
				
				Uint16 clientID = packet.c_data;
				if (clientID == NULL) {
					clientID = Server::getID();

					IPaddress ip;
					ip.port = packet.read<Uint16>();
					ip.host = packet.read<Uint32>();

					Log::info() << "Server. Proccesing new connection (" << clientID << ") " << ip.host << ":" << ip.port;

					if (ip.port == 0 || ip.host == 0) {
						Log::warning() << "A null IP was specified in the connection request. The request is ignored.";
						return;
					}

					for (Client client : self.clients)
						if (ip.host == client.ip.host && ip.port == client.ip.port) {
							Log::warning() << "The client is already connected, so will ignore the request.";
							return;
						}

					self.clients[clientID].ip = ip;

					Packet response = Packet(0);
					response.c_data = clientID;

					Server::Send(clientID, response);

					if (ConnectHandler)
						ConnectHandler(clientID);

					self.clients[clientID].lastReceiv = clock();

					return;
				} 

				if (clientID > self.clients.size())
					Log::warning() << "Server. Packet data received with invalid ID: " << clientID;

				self.clients[clientID].lastReceiv = clock();

				if (RequestHandler)
					RequestHandler(packet.c_data, &packet);

				SDLNet_FreePacket(udp_packet);
			}
		}

		for (Uint32 clientID = 1; clientID < self.clients.size(); ++clientID)
			if (self.clients[clientID].ip.host == 0 || self.clients[clientID].ip.port == 0)
				continue;
			else if (clock() > self.clients[clientID].lastReceiv + 3000) {
				Log::info() << "Net. Processing disconnect (" << clientID << ")";

				if (DisconnectHandler)
					DisconnectHandler(clientID);

				self.clients[clientID].ip.host = self.clients[clientID].ip.port = 0;
			} else if (clock() > self.clients[clientID].lastSend + 50) {
				Packet nonePacket = Packet(0);
				Server::Send(clientID, nonePacket);
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

	std::vector<Server::Client> Server::getClients() {
		return self.clients;
	}

	bool Server::warrantySend(Uint16 clientID, Packet packet) {
		if (!self.run) {
			Log::warning() << "Failed to send data because the server was not started";
			return false;
		}

		Uint32 num_ready;
		//for (Uint32 i = 0; i < 10; ++i) {
			Server::Send(clientID, packet);
			num_ready = SDLNet_CheckSockets(self.socket_set, 500);
			//if (num_ready > 0)
			//	break;
		//}

		if (num_ready > 0) {
			UDPpacket* package = Net::recieved(self.server_socket, 128);
			if (!package) {
				Log::error() << "Couldn't recieved data from client";
				return false;
			}

			SDLNet_FreePacket(package);
		}

		if (!num_ready)
			Log::error() << "warrantySend: Client takes too long to respond";

		return num_ready > 0;
	}

	void Server::Send(Uint16 clientID, Packet packet) {
		if (!self.run) {
			Log::warning() << "Failed to send data because the server was not started";
			return;
		}

		packet.packetID += 1;
		if (self.clients[clientID].lastPacketID > packet.packetID)
			packet.packetID = self.clients[clientID].lastPacketID;
		self.clients[clientID].lastPacketID = packet.packetID;
		self.clients[clientID].lastSend = clock();

		memcpy(&packet.data[0], &packet.code, 2);
		memcpy(&packet.data[2], &packet.packetID, 2);
		memcpy(&packet.data[4], &packet.c_data, 2);

		Net::send(self.server_socket, &self.clients[clientID].ip, packet.data, packet.len);
	}
}