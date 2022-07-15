#pragma once
#include "Client.h"

namespace EngineCore {
	class Server {
	private:
		bool run = false;

		UDPsocket server_socket;
		SDLNet_SocketSet socket_set;

		std::vector<IPaddress> clients;

		static Uint32 getID();

		static Server self;
	public:
		static void (*RequestHandler) (Uint16 code, byte* data, Uint32 len);
		static void (*ConnectHandler) (Uint32 id);

		static void Send(Uint32 id, byte* data, Uint32 len);

		static void start();
		static void update();
		static void free();
	};
}