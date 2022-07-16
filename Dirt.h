#pragma once
#include "Engine.h"

namespace Game {
	Uint16 NET_PLAYER_MOVE = 0;
	Uint16 NET_PLAYER_CREATE = 1;
	Uint16 NET_PLAYER_DESTROY = 2;

	class Dirt : public EngineCore::GameObject {
		void start() {
			builder("./asset/gameobjects/dirt.meta", this);

			sprite.position.x = 0;
			sprite.position.y = 7;
		}
	};
}