#pragma once
#include "TileMapObject.h"

namespace EngineCore {
	class Body {
	public:
		glm::vec2* position = NULL;
		glm::vec2 size = {1, 1};

		std::vector<TileMap*> tilemaps = {};

		void move(glm::vec2 velocity) {
			if (!position) return;

			bool canMove = true;

			glm::vec2 pos = *position;
			glm::vec2 directions[] = {
				{  size.x / 2, 0.f },
				{ -size.x / 2, 0.f },
				{ 0.f,  size.y / 2 },
				{ 0.f, -size.y / 2 }
			};


			pos.x += velocity.x;
			for (Uint32 i = 0; i < 4 && canMove; ++i) {
				for (Uint32 j = 0; j < tilemaps.size(); ++j) {
					int x = (int)((pos.x - tilemaps[j]->position.x + directions[i].x) / tilemaps[j]->scale.x);
					int y = (int)((pos.y - tilemaps[j]->position.y + directions[i].y) / tilemaps[j]->scale.y);

					if (x < 0 || y < 0 || x > tilemaps[j]->map.size() || y > tilemaps[j]->map[0].size())
						continue;

					if (tilemaps[j]->getTile(x, y).solid)
						canMove = false;
				}
			}

			if (canMove) position->operator=(pos);
			else pos.x -= velocity.x;

			canMove = true;

			pos.y += velocity.y;
			for (Uint32 i = 0; i < 4 && canMove; ++i) {
				for (Uint32 j = 0; j < tilemaps.size(); ++j) {
					int x = (int)((pos.x - tilemaps[j]->position.x + directions[i].x) / tilemaps[j]->scale.x);
					int y = (int)((pos.y - tilemaps[j]->position.y + directions[i].y) / tilemaps[j]->scale.y);

					if (x < 0 || y < 0 || x > tilemaps[j]->map.size() || y > tilemaps[j]->map[0].size())
						continue;

					if (tilemaps[j]->getTile(x, y).solid)
						canMove = false;
				}
			}

			if (canMove) position->operator=(pos);
			else pos.y -= velocity.y;
		}
	};
}