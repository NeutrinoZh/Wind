#pragma once
#include "Server.h"

namespace EngineCore {
	const Uint32 DISCONNECT = 9797;
	const Uint32 CONNECT = 9798;

	class Net {
	public:
		static bool isServer;

		static void send(UDPsocket socket, IPaddress* ip, byte* data, Uint32 len);
		static UDPpacket* recieved(UDPsocket socket, Uint32 len);

		static bool init();
		static void update();
		static void free();
	};
}