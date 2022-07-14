#pragma once
#include "Engine.h"

namespace Game {
	Uint16 NET_PLAYER_MOVE = 0;
	Uint16 NET_CREATE_PLAYER = 1;

	class Dirt : public EngineCore::GameObject {
		void start() {
			builder("./asset/gameobjects/dirt.meta", this);

			sprite.position.x = 0;
			sprite.position.y = 7;
		}
	};
}