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

			SDL_Surface* surface = SDL_CreateRGBSurface(NULL, 128, 128, 32, 0, 0, 0, 0);
			Uint32* pixels = (Uint32*)surface->pixels;

			for (Uint32 x = 0; x < 128; ++x)
				for (Uint32 y = 0; y < 128; ++y) {
					Uint32 color = 0;

					if (game().background->tilemap.map[x][y] == 5) color = SDL_MapRGBA(surface->format, 0, 0, 255, 255);
					if (game().background->tilemap.map[x][y] == 4) color = SDL_MapRGBA(surface->format, 132, 132, 132, 255);
					if (game().background->tilemap.map[x][y] == 3) color = SDL_MapRGBA(surface->format, 248, 235, 0, 255);
					if (game().background->tilemap.map[x][y] == 2) color = SDL_MapRGBA(surface->format, 0, 255, 0, 255);
					if (game().background->tilemap.map[x][y] == 1) color = SDL_MapRGBA(surface->format, 113, 42, 30, 255);

					pixels[x + (128 * y)] = color;
				}

			SDL_SaveBMP(surface, "./asset/map.png");
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

	EngineCore::Server::ConnectHandler = Game::GameServer::ConnectHandler;
	EngineCore::Server::DisconnectHandler = Game::GameServer::DisconnectHandler;
	EngineCore::Server::SendPacket = Game::GameServer::SendPacket;
	EngineCore::Server::GetPacket = Game::GameServer::GetPacket;

	EngineCore::Client::SendPacket = Game::GameClient::SendPacket;
	EngineCore::Client::GetPacket = Game::GameClient::GetPacket;

	return EngineCore::Core::loop();
};