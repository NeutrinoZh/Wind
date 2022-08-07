#pragma once
#include "Behavior.h"

namespace EngineCore {
	class TileMapObject : public Node {
	public:
		TileMap tilemap;
		Uint32 w = 14, h = 9;

		TileMapObject(TileMap tilemap): tilemap(tilemap) {};
	protected:
		void draw() {
			tilemap.draw(w, h);
		}
	};
}