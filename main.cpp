#include "GameServer.h"

namespace Game {
	void start() {
		game().background = new EngineCore::TileMapObject(
			EngineCore::TileMap::builder("./asset/tilemap/background.meta")
		);
		EngineCore::Core::scene->AddObject(game().background);
		
		game().foreground = new EngineCore::TileMapObject(
			EngineCore::TileMap::builder("./asset/tilemap/foreground.meta")
		);
		EngineCore::Core::scene->AddObject(game().foreground);

		game().chat->font = EngineCore::fonts()["PixelFont"];
		game().chat->position = { 0.1f, 0.5f };
		game().chat->Start();

		if (EngineCore::Net::isServer) {
			game().seed = time(0);
			mapGenerate("./asset/generator.meta");
		
			EngineCore::Window::delta = 0.f;
		}
	}

	void update() {
		game().chat->Update();
	}

	void draw() {
		game().chat->Draw();
	}
}

int main(int argc, char** args) {
	EngineCore::Core::Start  = Game::start;
	EngineCore::Core::Update = Game::update;
	EngineCore::Core::Draw   = Game::draw;

	EngineCore::Server::ConnectHandler	  = Game::GameServer::ConnectHandler;
	EngineCore::Server::DisconnectHandler = Game::GameServer::DisconnectHandler;
	EngineCore::Server::RequestHandler	  = Game::GameServer::RequestHandler;

	EngineCore::Client::SendData = Game::GameClient::SendData;
	EngineCore::Client::ResponseHandler	  = Game::GameClient::ResponseHandler;

	return EngineCore::Core::loop();
};