#pragma once
#include "Packet.h"

namespace EngineCore {
	class Client {
	private:
		bool run = false;

		IPaddress ip;
		UDPsocket client_socket;
		SDLNet_SocketSet socket_set;
		UDPpacket* packet;
		Uint32 id;

		static Client self;
	public:
		static void Send(Packet packet);

		static void update();
		static void connect();
		static void disconnect();
		static void free();
	};
}