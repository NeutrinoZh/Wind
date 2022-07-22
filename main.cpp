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

		//if (EngineCore::Net::isServer) {
		//	game().seed = time(0);
			mapGenerate("./asset/generator.meta");
		if (EngineCore::Net::isServer)
			EngineCore::Window::delta = 0.f;
		//}
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

	EngineCore::Server::ConnectHandler = Game::GameServer::ConnectHandler;
	EngineCore::Server::DisconnectHandler = Game::GameServer::DisconnectHandler;
	EngineCore::Server::SendPacket = Game::GameServer::SendPacket;
	EngineCore::Server::GetPacket = Game::GameServer::GetPacket;

	EngineCore::Client::SendPacket = Game::GameClient::SendPacket;
	EngineCore::Client::GetPacket = Game::GameClient::GetPacket;

	return EngineCore::Core::loop();
};