#include "Online.h"

namespace Game {
	void start() {
		EngineCore::Core::scene->AddObject(new Dirt());
	}
}

int main(int argc, char** args) {
	EngineCore::Core::Start = Game::start;

	EngineCore::Server::RequestHandler = Game::Online::RequestHandler;
	EngineCore::Server::ConnectHandler = Game::Online::ConnectHandler;
	EngineCore::Server::DisconnectHandler = Game::Online::DisconnectHandler;

	EngineCore::Client::ResponseHandler = Game::Online::ResponseHandler;

	return EngineCore::Core::loop();
};