#pragma once
#include "Packet.h"

namespace EngineCore {
	class Client {
	private:
		bool run = false;

		IPaddress ip;
		UDPpacket* packet;
		Uint32 id;

		Uint16  lastPacketID = 0;
		clock_t lastSend     = 0;
		clock_t lastReceiv   = 0;

	public:
		static Client self;
		SDLNet_SocketSet socket_set;
		UDPsocket client_socket;

		static void Send(Packet packet);
		static bool warrantySend(Packet packet);

		static Uint16 ID() {
			return self.id;
		}

		static void (*SendData) (void);
		static void (*ResponseHandler) (Packet* packet);

		static void update();
		static void connect();
		static void disconnect();
		static void free();
	};
}