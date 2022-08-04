#include "Net.h"

namespace EngineCore {
	bool Client::IsRepeat(Uint16 packetID) {
		if (!packetID) return false;

		auto it = std::find(self.to_ack.begin(), self.to_ack.end(), packetID);
		if (it != self.to_ack.end()) {
			self.lastReceiv = clock();
			return true;
		}

		return false;
	}
	void Client::ReadAck(Uint16 packetID) {
		if (!packetID) return;

		self.to_ack.push_back(packetID);
		if (self.to_ack.size() >= 17)
			self.to_ack.erase(self.to_ack.begin());
	}
	void Client::ReadBitfield(Packet* packet) {
		std::vector<Uint16> acks = { packet->ack };

		Uint16 bitfield = packet->bitfield;
		for (Uint16 j = 0; j < 16; ++j)
			if ((bitfield & (1 << j)) >> j)
				acks.push_back(packet->ack - (j + 1));

		for (Uint16 j = 0; j < self.sentPackets.size(); ++j) {
			auto it = std::find(acks.begin(), acks.end(), self.sentPackets[j].second.packetID);
			if (it != acks.end()) {
				self.sentPackets.erase(self.sentPackets.begin() + j);
				j -= 1;
			}
		}
	}
	void Client::ReadBigData(Packet* packet, void(*handler) (Packet*)) {
		Uint8 i = packet->read<Uint8>(),
			len = packet->read<Uint8>();

		if (!self.big_data.len) {
			self.big_data = Packet::create(246 * len, 0);
			self.big_data.code = 0;
			self.big_data.address = packet->address;
		}

		self.big_data.code += 1;

		memcpy(&self.big_data.data[i * 246], &packet->data[10], 246);

		if (self.big_data.code == len) {
			self.big_data.code = packet->code;
			if (handler)
				handler(&self.big_data);
			self.big_data.len = NULL;
		}
	}
	void Client::Read() {
		while (true) {
			Packet* packet = Client::read();
			if (!packet)
				break;

			if (Client::IsRepeat(packet->packetID)) {
				delete packet;
				continue;
			}
			else
				Client::ReadAck(packet->packetID);

			Client::ReadBitfield(packet);

			auto handler = Client::codeHandlers.find(packet->code);
			if (packet->len == 256)
				Client::ReadBigData(packet,
					(handler != Client::codeHandlers.end() ? handler->second : NULL)
				);
			else if (handler != Client::codeHandlers.end())
				handler->second(packet);

			self.lastReceiv = clock();
			delete packet;
		}
	}

	void Client::WriteAck() {
		if (self.to_ack.empty()) return;

		self.ack = *std::max_element(self.to_ack.begin(), self.to_ack.end());
		self.bitfield = NULL;

		for (Uint8 i = 0; i < 16; ++i) {
			auto it = std::find(self.to_ack.begin(), self.to_ack.end(), self.ack - i - 1);
			if (it != self.to_ack.end()) {
				self.bitfield = self.bitfield | (1 << i);
			}
		}
	}
	void Client::AddToSent() {
		for (Uint16 i = 0; i < self.sentPackets.size(); ++i)
			if (clock() > self.sentPackets[i].first + self.TIME_WAIT_FOR_RESEND) {
				self.sentPackets[i].first = clock();
				self.resend.push(self.sentPackets[i].second);
			}
	}
	void Client::Resend() {
		Client::send(self.resend.front());
		self.resend.pop();
	}
	void Client::SendFromQueue() {
		self.lastPacketID += 1;
		self.queue.front().packetID = self.lastPacketID;
		self.sentPackets.push_back(std::make_pair(
			clock(),
			self.queue.front()
		));

		Client::send(self.queue.front());
		self.queue.pop();

	}
	void Client::Send() {
		if (clock() > self.lastReceiv + self.TIME_WAIT_FOR_RESPONSE) {
			Log::info() << "Server not responding";
			self.disconnect();
		} else if (clock() > self.lastSend + self.DELTA_TIME_FOR_SEND) {
			self.lastSend = clock();

			Client::AddToSent();
			Client::WriteAck();

			if (!self.resend.empty()) 
				Client::Resend();
			else if (!self.queue.empty()) 
				Client::SendFromQueue();
			else {
				Packet packet;

				if (Client::SendPacket)
					packet = Client::SendPacket();
				else packet = Packet::create(0);

				Client::send(packet);
			}
		}
	}

	void Client::connect() {
		Log::info() << "Launched NetUDPClient";

		struct _config {
			std::string ip = "127.0.0.1";
			int port = 7708;

			clock_t TIME_WAIT_FOR_RESPONSE = 10000;
			clock_t DELTA_TIME_FOR_SEND = 50;
			clock_t TIME_WAIT_FOR_RESEND = 200;

			_config(std::string path) {
				Log::info() << "Read config NetUDPClient: " << path;

				Config config = ConfigReader::read(path);

				if (config.isVar("ip")) ip = config.getStringValue("ip");
				if (config.isVar("port")) port = config.getIntValue("port");
				if (config.isVar("TIME_WAIT_FOR_RESPONSE")) TIME_WAIT_FOR_RESPONSE = config.getIntValue("TIME_WAIT_FOR_RESPONSE");
				if (config.isVar("DELTA_TIME_FOR_SEND")) DELTA_TIME_FOR_SEND = config.getIntValue("DELTA_TIME_FOR_SEND");
				if (config.isVar("TIME_WAIT_FOR_RESEND")) TIME_WAIT_FOR_RESEND = config.getIntValue("TIME_WAIT_FOR_RESEND");
			}
		} config("./asset/connect.txt");

		self.socket = SDLNet_UDP_Open(0);
		if (self.socket == NULL) {
			Log::error() << SDLNet_GetError();
			return;
		}

		Log::info() << "Connect to NetUDPSerever on address " << config.ip << ":" << config.port;

		if (SDLNet_ResolveHost(&self.server, config.ip.c_str(), config.port) == -1) {
			Log::error() << SDLNet_GetError();
			return;
		}

		self.TIME_WAIT_FOR_RESPONSE = config.TIME_WAIT_FOR_RESPONSE;
		self.DELTA_TIME_FOR_SEND	= config.DELTA_TIME_FOR_SEND;
		self.TIME_WAIT_FOR_RESEND	= config.TIME_WAIT_FOR_RESEND;

		self.lastReceiv = clock();
		self.is_connect = true;
	}
	void Client::update() {
		if (!self.is_connect) return;

		Client::Read();
		Client::Send();
	}
	void Client::disconnect() {
		if (!self.is_connect) return;
		self.is_connect = false;

		Log::info() << "NetUDPClient disconnect";

		SDLNet_UDP_Close(self.socket);
	}
}