#pragma once
#include "Client.h"

namespace EngineCore {
	class Server {
	public:
		struct Client {
			std::queue<Packet> queue;

			IPaddress ip = IPaddress();
			Uint16  lastPacketID = 0;
			clock_t lastSend = 0;
			clock_t lastReceiv = 0;
		};
	private:
		static Uint16 getID();

		static void send(Uint16 clientID, Packet packet);
		static Packet* read();
	private:
		UDPsocket socket;
		bool is_run;

		std::vector<Client> clients = {};

		static Server self;
	public:
		static Packet(*SendPacket) (Uint16 clientID, Uint16 packetID);
		static void (*GetPacket) (Uint16 clientID, Packet* packet);

		static void (*ConnectHandler) (Uint16 clientID);
		static void (*DisconnectHandler) (Uint16 clientID);

		static void addToSend(Uint16 clientID, Packet packet);
		static void addToSendToEveryone(Packet packet);

		static void run();
		static void update();
		static void shutdown();
	};
}