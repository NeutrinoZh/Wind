#pragma once
#include "Tile.h"

namespace EngineCore {
	class TileMap {
	private:
		glm::mat4 transform = glm::mat4(1);
	public:
		Shader* shader = nullptr;

		glm::vec2 position = { 0, 1 },
				  scale    = { 1, 1 };
	public:
		std::vector<
			std::vector<Uint32>
		> map;

		std::vector<Tile> tiles;

		void addTile(Tile tile);

		TileMap(Uint32 x, Uint32 y);

		void draw();
		static TileMap builder(std::string path);
	};
}