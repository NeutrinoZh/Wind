#pragma once
#include "Textures.h"

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
		static void (*ResponseHandler) (byte* data, Uint32 len);

		static void Send(byte* data, Uint32 len);

		static void update();
		static void connect();
		static void disconnect();
		static void free();
	};
}