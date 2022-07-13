#pragma once
#include "Textures.h"

namespace EngineCore {
	class Server {
	private:
		bool run = false;
		
		UDPsocket server_socket;
		SDLNet_SocketSet socket_set;

		std::vector<IPaddress> clients;

		static Server self;

		static void close(UDPsocket);
	public:
		static void start();
		static void update();
		static void free();
	};

	class Client {
	private:
		bool run = false;

		IPaddress ip;
		UDPsocket client_socket;
		SDLNet_SocketSet socket_set;
		UDPpacket* packet;

		static Client self;
	public:
		static void connect();
		static void free();
	};

	class Net {
	public:
		static bool isServer;

		static void send(UDPsocket socket, IPaddress* ip, byte* data, Uint32 len);
		static UDPpacket* recieved(UDPsocket socket, SDLNet_SocketSet socket_set, Uint32 len);

		static bool init();
		static void update();
		static void free();
	};
}