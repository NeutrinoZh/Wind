#include "Net.h"
#pragma comment(lib, "Ws2_32.lib")

namespace EngineCore {
	bool Net::isServer = false;

	bool Net::init() {
		Log::info() << "SDL net init";

		if (SDLNet_Init() == -1) {
			Log::error() << "SDLNet_GetError: " << SDLNet_GetError();
			return false;
		}

		if (isServer) {
			Server::start();
		} else {
			Client::connect();
		}

		return true;
	}

	void Net::update() {
		if (isServer)
			Server::update();
		else Client::update();
	}

	void Net::free() {
		Log::info() << "Free memory from SDLNet";

		if (isServer)
			Server::free();
		else
			Client::free();

		SDLNet_Quit();
	}

	void Net::send(UDPsocket socket, IPaddress* ip, byte* data, Uint32 len) {
		if (ip->host == 0 && ip->port == 0) {
			Log::error() << "Couldn't send data to NULL ip " << ip->host << ":" << ip->port;
			return;
		}

		UDPpacket* packet = SDLNet_AllocPacket(len);

		if (!packet) {
			Log::error() << "Couldn't alloc UDP packet";
			return;
		}

		packet->address.host = ip->host;
		packet->address.port = ip->port;
		packet->len = len;

		memcpy(packet->data, data, len);

		if (
			SDLNet_UDP_Send(socket, -1, packet) != 1
			)
			Log::error() << "SDLNet_UDP_Send:" << SDLNet_GetError();
	}

	UDPpacket* Net::recieved(UDPsocket socket, Uint32 len) {
		UDPpacket* packet = SDLNet_AllocPacket(len);

		if (!packet) {
			Log::error() << "Couldn't alloc UDP packet";
			return NULL;
		}

		int result = SDLNet_UDP_Recv(socket, packet);

		if (result == -1) {
			Log::error() << SDLNet_GetError();
			return NULL;
		}

		if (result == 0)
			return NULL;

		return packet;
	}
}