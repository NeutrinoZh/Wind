#pragma once
#include "GameObject.h"

namespace EngineCore {
	class TileMapObject : public GameObject {
	public:
		TileMap tilemap;
	
		TileMapObject(TileMap tilemap): tilemap(tilemap) {};
	protected:
		void draw() {
			tilemap.draw();
		}
	};
}