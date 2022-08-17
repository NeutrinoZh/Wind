#pragma once
#include "Tile.h"

namespace WindEngine {
	class TileMap {
	private:
		glm::mat4 transform = glm::mat4(1);
	public:
		Shader* shader = nullptr;

		glm::vec2 position = { 0, 0 },
				  scale    = { 1, 1 };
	public:
		std::vector<
			std::vector<Uint32>
		> map;

		std::vector<Tile> tiles;

		void addTile(Tile tile);
		Tile getTile(Uint32 x, Uint32 y);

		TileMap(Uint32 x, Uint32 y);

		void draw(Uint32 w, Uint32 h);
	};
}