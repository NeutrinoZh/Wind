#pragma once
#include "Engine.h"

namespace Game {
	class Dirt : public EngineCore::GameObject {
		void start() {
			builder("./asset/gameobjects/dirt.meta", this);

			sprite.position.x = 0;
			sprite.position.y = 7;
		}
	};
}