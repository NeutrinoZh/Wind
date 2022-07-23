#include "Net.h"

namespace EngineCore {
	bool Net::isServer = true;

	bool Net::init() {
		Log::info() << "SDL net init";

		if (SDLNet_Init() == -1) {
			Log::error() << "SDLNet_GetError: " << SDLNet_GetError();
			return false;
		}

		if (isServer)
			Server::run();
		else
			Client::connect();

		return true;
	}

	void Net::update() {
		if (isServer)
			Server::update();
		else
			Client::update();
	}

	void Net::free() {
		Log::info() << "Free memory from SDLNet";

		if (isServer)
			Server::shutdown();
		else
			Client::disconnect();

		SDLNet_Quit();
	}
}