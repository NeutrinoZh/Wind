#pragma once
#include "Settings.h"

namespace Game {
	class Player : public EngineCore::GameObject {
	private:
		EngineCore::Animator animator;
		EngineCore::Body body;

		bool toMove;
		bool toSend = false;

		const float speed = 0.03;
	public:
		
		Uint32 ID;
		Uint8 state = 0;

		Player(Uint32 ID, bool toMove) : ID(ID), toMove(toMove) {}

		void start() {
			builder("./asset/gameobjects/player.meta", this);
			
			animator.sprite = &sprite;

			animator.add("player-run-left");
			animator.add("player-run-right");
			animator.add("player-run-top");
			animator.add("player-run-bottom");
			animator.add("player-stay-left");
			animator.add("player-stay-right");
			animator.add("player-stay-top");
			animator.add("player-stay-bottom");

			animator.set("player-run-left");
		
			body.position = &sprite.position;
			body.size = { 3/16.f, 5/16.f };
			body.tilemaps.push_back(&game().background->tilemap);
			body.tilemaps.push_back(&game().foreground->tilemap);
		}

		void update() {
			animator.update();

			if (toMove) {
				glm::vec2 velocity = { 0, 0 };

				if      (EngineCore::Keyboard::isPressed(SDLK_w)) { velocity.y =  delta(speed); state = 0; }
				else if (EngineCore::Keyboard::isPressed(SDLK_s)) { velocity.y = -delta(speed); state = 2; }
				
				if      (EngineCore::Keyboard::isPressed(SDLK_a)) { velocity.x = -delta(speed); state = 1; }
				else if (EngineCore::Keyboard::isPressed(SDLK_d)) { velocity.x =  delta(speed); state = 3; }

				if (velocity.x == 0 && velocity.y == 0.f && state < 4) {
					state += 4;
				}

				body.move(velocity);

				float sx = EngineCore::Window::size.x / 200,
					  sy = EngineCore::Window::size.y / 200;

				glm::vec2 cameraPosition = sprite.position - glm::vec2(
					sx-sprite.scale.x/2+sprite.origin.x,
					sy+sprite.scale.y/2+sprite.origin.y
				);

				EngineCore::camera().position = cameraPosition;
			}

			if (state == 0) animator.set("player-run-top");
			if (state == 1) animator.set("player-run-left");
			if (state == 2) animator.set("player-run-bottom");
			if (state == 3) animator.set("player-run-right");

			if (state == 4) animator.set("player-stay-top");
			if (state == 5) animator.set("player-stay-left");
			if (state == 6) animator.set("player-stay-bottom");
			if (state == 7) animator.set("player-stay-right");

		}

		void netUpdate() {

		}
	};
}