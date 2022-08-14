#include "Net.h"

namespace EngineCore {
	bool Net::isServer = false;

	bool Net::init(JText::Object& config) {
		Log::info() << "Net-System init";

		if (!SDLSystems::NET) {
			Log::error() << "Net-System requires SDL_Net";
			return false;
		}

		isServer = config.is();

		if (isServer)
			Server::run(config);

		return true;
	}

	void Net::update() {
		if (isServer)
			Server::update();
		else
			Client::update();
	}

	void Net::free() {
		Log::info() << "Free memory from Net-System";

		if (isServer)
			Server::shutdown();
		else
			Client::disconnect();
	}
}