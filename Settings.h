#pragma once
#include "Engine.h"

namespace Game {
	struct Game {
		const Uint16 NET_PLAYER_CREATE = 1,
				 	 NET_PLAYER_MOVE = 2,
					 NET_PLAYER_DESTROY = 3,
					 NET_MAP_GENERATE = 4;

		EngineCore::Chat* chat = new EngineCore::Chat();
		Uint32 seed = 0;

		EngineCore::TileMapObject* background;
		EngineCore::TileMapObject* foreground;

		std::vector<EngineCore::GameObject*> players;
		Uint16 playerID = NULL;

		std::string path = "";

		static Game* game;
	};

	Game& game();
}