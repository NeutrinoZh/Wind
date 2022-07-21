#pragma once
#include "Client.h"

namespace EngineCore {
	class Server {
	public:
		struct Client {
			IPaddress ip = IPaddress();
			Uint16 lastPacketID = 0;
			clock_t lastSend = 0;
			clock_t lastReceiv = 0;
		};
	private:
		bool run = false;

		SDLNet_SocketSet socket_set;

		std::vector<Client> clients = { Client() };

		static Uint16 getID();

	public:
		UDPsocket server_socket;
		static Server self;

		static void (*ConnectHandler) (Uint32 id);
		static void (*DisconnectHandler) (Uint32 id);
		static void (*RequestHandler) (Uint32 id, Packet* packet);

		static std::vector<Client> getClients();
		static void Send(Uint16 clientID, Packet packet);
		static bool warrantySend(Uint16 clientID, Packet packet);

		static void start();
		static void update();
		static void free();
	};
}