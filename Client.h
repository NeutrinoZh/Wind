#pragma once
#include "Packet.h"

namespace EngineCore {
	class Client {
	private:
		static void send(Packet packet);
		static Packet* read();
	private:
		UDPsocket socket;

		IPaddress server;
		Uint16  lastPacketID = 0;
		clock_t lastSend = 0;
		clock_t lastReceiv = 0;

		std::vector<std::pair<clock_t, Packet>> sentPackets;
		std::queue<Packet> resend;

		std::vector<Uint16> to_ack;
		std::queue<Packet> queue;

		Uint16 ack = NULL;
		Uint16 bitfield = NULL;

		Packet big_data;

	public:
		bool is_connect = false;
		static Client self;

		static Packet(*SendPacket) (void);
		static void (*GetPacket) (Packet* packet);

		static void addToSend(Packet packet);

		static void connect();
		static void update();
		static void disconnect();
	};
}