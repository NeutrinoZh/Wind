#include "Online.h"

namespace Game {
	void start() {
		background = new EngineCore::TileMapObject(
			EngineCore::TileMap::builder("./asset/tilemap/background.meta")
		);
		EngineCore::Core::scene->AddObject(background);
		
		foreground = new EngineCore::TileMapObject(
			EngineCore::TileMap::builder("./asset/tilemap/foreground.meta")
		);
		EngineCore::Core::scene->AddObject(foreground);

		if (EngineCore::Net::isServer) {
			seed = time(0);

			mapGenerate("./asset/generator.meta");
			
			SDL_Surface* surface = SDL_CreateRGBSurface(NULL, 128, 128, 32, 0, 0, 0, 0);
			Uint32* pixels = (Uint32*)surface->pixels;

			for (Uint32 x = 0; x < 128; ++x)
				for (Uint32 y = 0; y < 128; ++y) {
					Uint32 color = 0;

					if (background->tilemap.map[x][y] == 5) color = SDL_MapRGBA(surface->format, 0, 0, 255, 255);
					if (background->tilemap.map[x][y] == 4) color = SDL_MapRGBA(surface->format, 132, 132, 132, 255);
					if (background->tilemap.map[x][y] == 3) color = SDL_MapRGBA(surface->format, 248, 235, 0, 255);
					if (background->tilemap.map[x][y] == 2) color = SDL_MapRGBA(surface->format, 0, 255, 0, 255);
					if (background->tilemap.map[x][y] == 1) color = SDL_MapRGBA(surface->format, 113, 42, 30, 255);

					pixels[x + (128 * y)] = color;
				}

			SDL_SaveBMP(surface, "./asset/map.png");
		}
	}

	clock_t lastDestroy = 0;

	void update() {
		for (Uint32 i = 0; i < chat.size(); ++i) 
			chat[i]->Update();

		if (chat.empty())
			lastDestroy = clock();
		if (clock() > lastDestroy + 5000) {
			chat[0]->Free();
			chat.erase(chat.begin());
			lastDestroy = clock();
		}
	}

	void draw() {
		for (Uint32 i = 0; i < chat.size(); ++i)
			chat[i]->Draw();
	}
}

int main(int argc, char** args) {
	EngineCore::Core::Start  = Game::start;
	EngineCore::Core::Update = Game::update;
	EngineCore::Core::Draw   = Game::draw;

	EngineCore::Server::RequestHandler = Game::Online::RequestHandler;
	EngineCore::Server::ConnectHandler = Game::Online::ConnectHandler;
	EngineCore::Server::DisconnectHandler = Game::Online::DisconnectHandler;

	EngineCore::Client::ResponseHandler = Game::Online::ResponseHandler;

	return EngineCore::Core::loop();
};