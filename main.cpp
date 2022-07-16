#include "Online.h"

namespace Game {
	void start() {
		tilemap = new EngineCore::TileMapObject(
			EngineCore::TileMap::builder("./asset/tilemap/tilemap.meta")
		);
		EngineCore::Core::scene->AddObject(tilemap);
		
		if (EngineCore::Net::isServer)
			seed = time(0); 
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