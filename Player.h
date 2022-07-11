#pragma once
#include "Dirt.h"

namespace Game {
	class Player : public EngineCore::GameObject {
		void start() {
			builder("./asset/gameobjects/player.meta", this);

			sprite.position.x = 2;
			sprite.position.y = 2;
		}

		void update() {
			if (EngineCore::Keyboard::isPressed(SDLK_w)) sprite.position.y += 0.03f;
			if (EngineCore::Keyboard::isPressed(SDLK_a)) sprite.position.x -= 0.03f;
			if (EngineCore::Keyboard::isPressed(SDLK_s)) sprite.position.y -= 0.03f;
			if (EngineCore::Keyboard::isPressed(SDLK_d)) sprite.position.x += 0.03f;
		}
	};
}