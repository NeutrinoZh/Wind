#include "Net.h"

namespace EngineCore {
	Uint16 Server::getFreeID() {
		for (Uint32 i = 0; i < self.clients.size(); ++i)
			if (
				self.clients[i].ip.host == 0 &&
				self.clients[i].ip.port == 0
				) return i;

		self.clients.push_back(Client());
		return (Uint16)(self.clients.size() - 1);
	}
	Uint16 Server::getID(IPaddress ip) {
		for (Uint32 i = 0; i < self.clients.size(); ++i)
			if (
				self.clients[i].ip.host == ip.host &&
				self.clients[i].ip.port == ip.port
			) return i;
		return UINT16_MAX;
	}
	void Server::ReadAcks(Uint16 id, Packet* packet) {
		std::vector<Uint16> acks = { packet->ack };

		Uint16 bitfield = packet->bitfield;
		for (Uint16 j = 0; j < 16; ++j)
			if ((bitfield & (1 << j)) >> j)
				acks.push_back(packet->ack - (j + 1));

		for (Uint16 j = 0; j < self.clients[id].sentPackets.size(); ++j) {
			auto it = std::find(acks.begin(), acks.end(), self.clients[id].sentPackets[j].second.packetID);
			if (it != acks.end()) {
				self.clients[id].sentPackets.erase(self.clients[id].sentPackets.begin() + j);
				j -= 1;
			}
		}
	}
	bool Server::IsRepeat(Uint16 id, Packet* packet) {
		if (!packet->packetID) return false;

		auto it = std::find(self.clients[id].to_ack.begin(), self.clients[id].to_ack.end(), packet->packetID);
		return (it != self.clients[id].to_ack.end());
	}
	void Server::ReadAck(Uint16 id, Packet* packet) {
		self.clients[id].to_ack.push_back(packet->packetID);
		if (self.clients[id].to_ack.size() >= 17)
			self.clients[id].to_ack.erase(self.clients[id].to_ack.begin());
	}
	void Server::InvokeHandler(Uint16 id, Packet* packet) {
		auto handler = Server::codeHandlers.find(packet->code);
		if (handler != Server::codeHandlers.end())
			handler->second(id, packet);
	}
	bool Server::Connect(Uint16 id, Packet* packet) {
		Uint32 clientID = getFreeID();

		Log::info() << "Server. Proccesing new connection (" << clientID << ") " <<
			packet->address.host << ":" << packet->address.port;

		self.clients[clientID].ip = packet->address;
		self.clients[clientID].lastReceiv = clock();

		if (Server::IsRepeat(id, packet)) {
			self.clients[id].lastReceiv = clock();
			return false;
		} else
			Server::ReadAck(id, packet);

		if (Server::ConnectHandler)
			Server::ConnectHandler(clientID);

		Server::InvokeHandler(id, packet);
		return true;
	}
	void Server::Read() {
		while (true) {
			Packet* packet = Server::read();
			if (!packet)
				break;

			Uint16 id = Server::getID(packet->address);
			if (id != UINT16_MAX) {
				Server::ReadAcks(id, packet);

				if (Server::IsRepeat(id, packet)) {
					self.clients[id].lastReceiv = clock();

					delete packet;
					continue;
				} else
					Server::ReadAck(id, packet);

				Server::InvokeHandler(id, packet);

				self.clients[id].lastReceiv = clock();
			} else if (!Server::Connect(id, packet)) {
				delete packet;
				continue;
			}

			delete packet;
		}
	}

	void Server::AddToResend(Uint16 id) {
		for (Uint16 i = 0; i < self.clients[id].sentPackets.size(); ++i)
			if (clock() > self.clients[id].sentPackets[i].first + self.TIME_WAIT_FOR_RESEND) {
				self.clients[id].sentPackets[i].first = clock();
				self.clients[id].resend.push(self.clients[id].sentPackets[i].second);
			}
	}
	void Server::Disconnect(Uint16 id) {
		Log::info() << "Net. Processing disconnect (" << id << ")";

		if (Server::DisconnectHandler)
			Server::DisconnectHandler(id);

		self.clients[id].ip.host = self.clients[id].ip.port = 0;
	}
	void Server::WriteBitfield(Uint16 id) {
		if (!self.clients[id].to_ack.empty()) {
			self.clients[id].ack = *std::max_element(self.clients[id].to_ack.begin(), self.clients[id].to_ack.end());
			self.clients[id].bitfield = NULL;

			for (Uint8 i = 0; i < 16; ++i) {
				auto it = std::find(self.clients[id].to_ack.begin(), self.clients[id].to_ack.end(), self.clients[id].ack - i - 1);
				if (it != self.clients[id].to_ack.end())
					self.clients[id].bitfield = self.clients[id].bitfield | (1 << i);
			}
		}
	}
	void Server::Resend(Uint16 id) {
		Server::send(id, self.clients[id].resend.front());
		self.clients[id].resend.pop();
	}
	void Server::SendFromQueue(Uint16 id) {
		self.clients[id].lastPacketID += 1;
		self.clients[id].queue.front().packetID = self.clients[id].lastPacketID;
		self.clients[id].sentPackets.push_back(std::make_pair(
			clock(),
			self.clients[id].queue.front()
		));

		Server::send(id, self.clients[id].queue.front());
		self.clients[id].queue.pop();
	}
	void Server::Send() {
		for (Uint32 clientID = 0; clientID < self.clients.size(); ++clientID) {
			if (self.clients[clientID].ip.host == 0 || self.clients[clientID].ip.port == 0)
				continue;

			Server::AddToResend(clientID);

			if (clock() > self.clients[clientID].lastReceiv + self.TIME_WAIT_FOR_RESPONSE)
				Server::Disconnect(clientID);
			else if (clock() > self.clients[clientID].lastSend + self.DELTA_TIME_FOR_SEND) {
				self.clients[clientID].lastSend = clock();

				Server::WriteBitfield(clientID);

				if (!self.clients[clientID].resend.empty())
					Server::Resend(clientID);
				else if (!self.clients[clientID].queue.empty())
					Server::SendFromQueue(clientID);
				else {
					Packet packet;

					if (Server::SendPacket)
						packet = Server::SendPacket(clientID);
					else packet = Packet::create(0);

					Server::send(clientID, packet);
				}
			}
		}
	}

	void Server::run() {
		Log::info() << "Launched NetUDPServer";

		struct _config {
			int port = 7708;

			clock_t TIME_WAIT_FOR_RESPONSE = 2000;
			clock_t DELTA_TIME_FOR_SEND = 50;
			clock_t TIME_WAIT_FOR_RESEND = 200;

			_config(std::string path) {
				Log::info() << "Read config NetUDPServer: " << path;

				Config config = ConfigReader::read(path);

				if (config.isVar("port")) port = config.getIntValue("port");

				if (config.isVar("TIME_WAIT_FOR_RESPONSE")) TIME_WAIT_FOR_RESPONSE = config.getIntValue("TIME_WAIT_FOR_RESPONSE");
				if (config.isVar("DELTA_TIME_FOR_SEND"))	DELTA_TIME_FOR_SEND	   = config.getIntValue("DELTA_TIME_FOR_SEND");
				if (config.isVar("TIME_WAIT_FOR_RESEND"))	TIME_WAIT_FOR_RESEND   = config.getIntValue("TIME_WAIT_FOR_RESEND");
			}
		} config("./asset/server.txt");

		self.socket = SDLNet_UDP_Open(config.port);
		if (self.socket == NULL) {
			Log::error() << "SDLNet_UDP_Open: " << SDLNet_GetError();
			return;
		}

		self.TIME_WAIT_FOR_RESPONSE = config.TIME_WAIT_FOR_RESPONSE;
		self.DELTA_TIME_FOR_SEND    = config.DELTA_TIME_FOR_SEND;
		self.TIME_WAIT_FOR_RESEND   = config.TIME_WAIT_FOR_RESEND;

		self.is_run = true;
	}
	void Server::update() {
		if (!self.is_run) return;

		Server::Read();
		Server::Send();
	}
	void Server::shutdown() {
		if (!self.is_run) return;

		Log::info() << "Shutdown NetUDPServer";

		SDLNet_UDP_Close(self.socket);
	}
}