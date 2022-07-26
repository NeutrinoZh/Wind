#include "MainMenu.h"
#include <conio.h>

namespace Game {
	void start() {
		game().seed = time(NULL);

		if (EngineCore::Net::isServer) {
			EngineCore::Window::delta = 0.f;

			game().background = new EngineCore::TileMapObject(
				EngineCore::TileMap::builder("./asset/tilemap/background.meta")
			);
			EngineCore::Core::scene->AddObject(game().background);

			game().foreground = new EngineCore::TileMapObject(
				EngineCore::TileMap::builder("./asset/tilemap/foreground.meta")
			);
			EngineCore::Core::scene->AddObject(game().foreground);
	
			mapGenerate("./asset/generator.meta");
			saveMapToImage("./asset/map.png");
		} else {
			game().chat->font = EngineCore::fonts()["PixelFont"];
			game().chat->position = { 0.1f, 0.5f };
			game().chat->Start();

			MainMenu::Start();
		}
	}

	void update() {
		if (EngineCore::Net::isServer) {
			if (_kbhit())
				if (_getch() == 'e')
					EngineCore::Window::quit();

			if (EngineCore::Keyboard::isPressed(SDLK_w)) EngineCore::camera().position.y += delta(0.03);
			if (EngineCore::Keyboard::isPressed(SDLK_s)) EngineCore::camera().position.y -= delta(0.03);
			if (EngineCore::Keyboard::isPressed(SDLK_a)) EngineCore::camera().position.x -= delta(0.03);
			if (EngineCore::Keyboard::isPressed(SDLK_d)) EngineCore::camera().position.x += delta(0.03);
		} else {
			if (!EngineCore::Client::self.is_connect)
				MainMenu::self.container->active = true;

			if (EngineCore::Keyboard::isPressed(SDLK_ESCAPE) && EngineCore::Client::self.is_connect)
				EngineCore::Client::disconnect();

			game().chat->Update();
			MainMenu::Update();
		}
	}

	void draw() {
		if (!EngineCore::Net::isServer) {
			game().chat->Draw();
			MainMenu::Draw();
		}
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
	EngineCore::Client::GetPacket = Game::GameClient::GetPacket;

	return EngineCore::Core::loop();
};