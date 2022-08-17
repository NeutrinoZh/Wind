#include "TileMap.h"

namespace WindEngine {
	TileMap::TileMap(Uint32 x, Uint32 y) {
		map.resize(x);
		for (Uint32 i = 0; i < map.size(); ++i)
			map[i].resize(y);
	}

	void TileMap::draw(Uint32 w, Uint32 h) {
		if (!shader) {
			Log::error() << "When tilmap drawing shader been NULL";
			return;
		}

		int cx = static_cast<int>(camera().position.x - position.x),
			cy = static_cast<int>(camera().position.y - position.y);

		transform = glm::mat4(1);
		transform = glm::scale(transform, glm::vec3(scale, 1.0f));
		transform = glm::translate(transform, glm::vec3(position - camera().position, 0));

		float verticesQuad[16] = {
			0,  0,  0, 0,
			1,  0,  1, 0,
			0, -1,  0, 1,
			1, -1,  1, 1
		};

		for (Uint32 x = 0; x < map.size(); ++x) {
			for (Uint32 y = 0; y < map[x].size(); ++y) {
				if (y != 0)
					transform = glm::translate(transform, glm::vec3(0, scale.y, 0));

				if (!(x >= cx && x < cx + w && y > cy && y <= cy + h))
					continue;

				if (map[x][y] == NULL)
					continue;
				
				Texture texture = tiles[map[x][y] - 1].texture;

				verticesQuad[2] = texture.rect.x;
				verticesQuad[6] = texture.rect.z;
				verticesQuad[10] = texture.rect.x;
				verticesQuad[14] = texture.rect.z;

				verticesQuad[3] = texture.rect.y;
				verticesQuad[7] = texture.rect.y;
				verticesQuad[11] = texture.rect.w;
				verticesQuad[15] = texture.rect.w;

				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, verticesQuad, GL_DYNAMIC_DRAW);

				glBindTexture(GL_TEXTURE_2D, texture.texture);
				
				shader->use();
				shader->setMat("Proj", GL_Context::proj);
				shader->setMat("Transform", transform);
				shader->setVec4f("Color", tiles[map[x][y] - 1].color);

				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}

			transform = glm::translate(transform, glm::vec3(scale.x, -(scale.y * (map[x].size() - 1)), 0));
		}
	}

	Tile TileMap::getTile(Uint32 x, Uint32 y) {
		if (map[x][y] != NULL)
			return tiles[map[x][y] - 1];
		return Tile();
	}

	void TileMap::addTile(Tile tile) {
		tiles.push_back(tile);
	}
}