#pragma once
#include "Client.h"

namespace EngineCore {
	class Server {
	private:
		bool run = false;

		UDPsocket server_socket;
		SDLNet_SocketSet socket_set;

		std::vector<IPaddress> clients = { IPaddress() };

		static Uint16 getID();

		static Server self;
	public:
		static void (*ConnectHandler) (Uint32 id);
		static void (*DisconnectHandler) (Uint32 id);

		static std::vector<IPaddress> getClients();
		static void Send(Uint16 clientID, Packet packet);

		static void start();
		static void update();
		static void free();
	};
}