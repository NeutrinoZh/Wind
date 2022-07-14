#pragma once
#include "Textures.h"

namespace EngineCore {
	class Server {
	private:
		bool run = false;
		
		UDPsocket server_socket;
		SDLNet_SocketSet socket_set;

		std::vector<IPaddress> clients;

		static Uint32 getID();
		
		static Server self;
	public:
		static void (*RequestHandler) (Uint16 code, byte* data, Uint32 len);
		static void (*ConnectHandler) (Uint32 id);

		static void Send(Uint32 id, byte* data, Uint32 len);
		
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
		Uint32 id;

		static Client self;
	public:
		static void (*ResponseHandler) (byte* data, Uint32 len);

		static void Send(byte* data, Uint32 len);

		static void update();
		static void connect();
		static void disconnect();
		static void free();
	};

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