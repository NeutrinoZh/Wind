#include "Net.h"

namespace EngineCore {
	Server Server::self = Server();
	void (*Server::ConnectHandler)	  (Uint32 id) = NULL;
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

	Uint16 Server::getID() {
		for (Uint32 i = 1; i < self.clients.size(); ++i)
			if (
				self.clients[i].host == 0 &&
				self.clients[i].port == 0
				) return i;

		self.clients.push_back(IPaddress());
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

					self.clients[clientID] = ip;

					Packet response = Packet(2);
					response.write<Uint16>(clientID);
					response.packetID = packet.packetID + 1;

					Server::Send(clientID, response);
				} 

				if (clientID > self.clients.size())
					Log::warning() << "Server. Packet data received with invalid ID: " << clientID;

				SDLNet_FreePacket(udp_packet);
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

	void Server::Send(Uint16 clientID, Packet packet) {
		if (!self.run) {
			Log::warning() << "Failed to send data because the server was not started";
			return;
		}

		memcpy(&packet.data[0], &packet.code, 2);
		memcpy(&packet.data[2], &packet.packetID, 2);
		memcpy(&packet.data[4], &packet.c_data, 2);

		Net::send(self.server_socket, &self.clients[clientID], packet.data, packet.len);
	}
}