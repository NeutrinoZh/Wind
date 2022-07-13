#pragma once
#include "Dirt.h"

namespace Game {
	class Player : public EngineCore::GameObject {
	private:
		EngineCore::Animation left, right, top, bottom;

		EngineCore::Animation* current = nullptr;
	public:
		void start() {
			builder("./asset/gameobjects/player.meta", this);
			
			left = EngineCore::animations()["player-run-left"];
			left.sprite = &sprite;

			right = EngineCore::animations()["player-run-right"];
			right.sprite = &sprite;

			top = EngineCore::animations()["player-run-top"];
			top.sprite = &sprite;

			bottom = EngineCore::animations()["player-run-bottom"];
			bottom.sprite = &sprite;

			current = &left;

			sprite.position.x = 2;
			sprite.position.y = 2;
		}

		void update() {
			current->update();

			if (EngineCore::Keyboard::isPressed(SDLK_w)) { sprite.position.y += 0.03f; current = &top;    }
			if (EngineCore::Keyboard::isPressed(SDLK_a)) { sprite.position.x -= 0.03f; current = &left;   }
			if (EngineCore::Keyboard::isPressed(SDLK_s)) { sprite.position.y -= 0.03f; current = &bottom; }
			if (EngineCore::Keyboard::isPressed(SDLK_d)) { sprite.position.x += 0.03f; current = &right;  }
		}
	};
}