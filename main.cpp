#include "MainMenu.h"
#include <conio.h>

namespace Game {
	void start() {
		game().seed = time(NULL);

		//=============================================================//

		EngineCore::GameObject* gameScene = new EngineCore::GameObject(); {
			game().background = new EngineCore::TileMapObject(
				EngineCore::TileMap::builder("./asset/tilemap/background.meta")
			);
			gameScene->AddObject(game().background);

			game().foreground = new EngineCore::TileMapObject(
				EngineCore::TileMap::builder("./asset/tilemap/foreground.meta")
			);
			gameScene->AddObject(game().foreground);
		} EngineCore::Core::addScene("game", gameScene);

		EngineCore::GameObject* menuScene = new EngineCore::GameObject(); {
			menuScene->AddObject(new MainMenu());
		} EngineCore::Core::addScene("menu", menuScene);

		//=============================================================//

		if (EngineCore::Net::isServer) {
			EngineCore::Core::setScene("game");
			EngineCore::Window::delta = 0.f;

			mapGenerate("./asset/generator.meta");
			saveMapToImage("./asset/map.png");
		} else {
			EngineCore::Core::setScene("menu");

			game().chat->font = EngineCore::fonts()["PixelFont"];
			game().chat->position = { 0.1f, 0.5f };
			game().chat->Start();
		}
	}

	void update() {
		if (EngineCore::Net::isServer) {
			if (_kbhit())
				if (_getch() == 'e')
					EngineCore::Window::quit();

			if (EngineCore::Keyboard::isPressed(SDLK_w)) EngineCore::camera().position.y += delta(0.09);
			if (EngineCore::Keyboard::isPressed(SDLK_s)) EngineCore::camera().position.y -= delta(0.09);
			if (EngineCore::Keyboard::isPressed(SDLK_a)) EngineCore::camera().position.x -= delta(0.09);
			if (EngineCore::Keyboard::isPressed(SDLK_d)) EngineCore::camera().position.x += delta(0.09);
		} else {
			if (!EngineCore::Client::isConnect())
				EngineCore::Core::setScene("menu");

			if (EngineCore::Keyboard::isPressed(SDLK_ESCAPE) && EngineCore::Client::isConnect())
				EngineCore::Client::disconnect();

			game().chat->Update();
		}
	}

	void draw() {
		if (!EngineCore::Net::isServer)
			game().chat->Draw();
	}
}

int main(int argc, char** args) {
	Game::game().path = args[0];
	EngineCore::Net::isServer = (argc > 1);

	EngineCore::Core::Start  = Game::start;
	EngineCore::Core::Update = Game::update;
	EngineCore::Core::Draw   = Game::draw;

	EngineCore::Server::ConnectHandler = Game::GameServer::ConnectHandler;
	EngineCore::Server::DisconnectHandler = Game::GameServer::DisconnectHandler;
	EngineCore::Server::SendPacket = Game::GameServer::SendPacket;
	EngineCore::Server::GetPacket = Game::GameServer::GetPacket;

	EngineCore::Client::SendPacket = Game::GameClient::SendPacket;

	EngineCore::Client::addCodeHandler(Game::game().NET_PLAYER_CREATE, Game::GameClient::PlayerCreate);
	EngineCore::Client::addCodeHandler(Game::game().NET_PLAYER_MOVE, Game::GameClient::PlayerMove);
	EngineCore::Client::addCodeHandler(Game::game().NET_PLAYER_DESTROY, Game::GameClient::PlayerDestroy);
	EngineCore::Client::addCodeHandler(Game::game().NET_MAP_GENERATE, Game::GameClient::MapGenerate);

	return EngineCore::Core::loop();
};