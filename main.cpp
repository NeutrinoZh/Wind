#include "Online.h"

namespace Game {
	void start() {
		EngineCore::scene->AddObject(new Dirt());
	}
}

int main(int argc, char** args) {
	EngineCore::Start = Game::start;

	EngineCore::Server::RequestHandler = Game::Online::RequestHandler;
	EngineCore::Server::ConnectHandler = Game::Online::ConnectHandler;

	EngineCore::Client::ResponseHandler = Game::Online::ResponseHandler;

	return EngineCore::loop();
};