#pragma once
#include "Packet.h"

namespace EngineCore {
	class Client {
	private:
		static bool IsRepeat(Uint16 packetID);
		static void ReadAck(Uint16 packetID);
		static void ReadBitfield(Packet* packet);
		static void ReadBigData(Packet* packet, void(*handler) (Packet*));
		static void Read();

		static void AddToSent();
		static void WriteAck();
		static void Resend();
		static void SendFromQueue();
		static void Send();

		static void send(Packet packet);
		static Packet* read();
	private:
		UDPsocket socket;

		IPaddress server;
		Uint16  lastPacketID = 0;
		clock_t lastSend = 0;
		clock_t lastReceiv = 0;

		clock_t TIME_WAIT_FOR_RESPONSE = 10000;
		clock_t DELTA_TIME_FOR_SEND	   = 50;
		clock_t TIME_WAIT_FOR_RESEND   = 200;

		std::vector<std::pair<clock_t, Packet>> sentPackets;
		std::queue<Packet> resend;

		std::vector<Uint16> to_ack;
		std::queue<Packet> queue;

		Uint16 ack = NULL;
		Uint16 bitfield = NULL;

		Packet big_data;

		bool is_connect = false;
		static Client self;

		static std::map<Uint16, void(*) (Packet*)> codeHandlers;
	public:
		static Packet(*SendPacket) (void);

		static void addToSend(Packet packet);
		static void addCodeHandler(Uint16 code, void(*handler) (Packet*));
		static bool isConnect();

		static void connect();
		static void update();
		static void disconnect();
	};
}