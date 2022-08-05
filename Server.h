#pragma once
#include "Client.h"

namespace EngineCore {
	class Server {
	public:
		struct Client {
			std::queue<Packet> queue;
			std::vector<std::pair<clock_t, Packet>> sentPackets;
			std::queue<Packet> resend;
			std::vector<Uint16> to_ack;

			Uint16 ack = NULL;
			Uint16 bitfield = NULL;

			IPaddress ip = IPaddress();
			Uint16  lastPacketID = 0;
			clock_t lastSend = 0;
			clock_t lastReceiv = 0;
		};
	private:
		static Uint16 getFreeID();

		static Uint16 getID(IPaddress ip);
		static void ReadAck(Uint16 id, Packet* packet);
		static void ReadAcks(Uint16 id, Packet* packet);
		static bool IsRepeat(Uint16 id, Packet* packet);
		static void InvokeHandler(Uint16 id, Packet* packet);
		static bool Connect(Uint16 id, Packet* packet);
		static void Read();

		static void AddToResend(Uint16 id);
		static void Disconnect(Uint16 id);
		static void WriteBitfield(Uint16 id);
		static void Resend(Uint16 id);
		static void SendFromQueue(Uint16 id);
		static void Send();

		static void send(Uint16 clientID, Packet packet);
		static Packet* read();
	private:
		UDPsocket socket;
		bool is_run;

		clock_t TIME_WAIT_FOR_RESPONSE = 2000;
		clock_t DELTA_TIME_FOR_SEND = 50;
		clock_t TIME_WAIT_FOR_RESEND = 200;

		std::vector<Client> clients = {};

		static Server self;
		static std::map<Uint16, void (*)(Uint16, Packet*)> codeHandlers;
	public:
		static Packet(*SendPacket) (Uint16 clientID);

		static void (*ConnectHandler) (Uint16 clientID);
		static void (*DisconnectHandler) (Uint16 clientID);

		static void addCodeHandler(Uint16 code, void(*handler) (Uint16, Packet*));

		static void addToSendData(Uint16 clientID, Packet data);
		static void addToSend(Uint16 clientID, Packet packet);
		static void addToSendToEveryone(Packet packet);

		static void run();
		static void update();
		static void shutdown();
	};
}