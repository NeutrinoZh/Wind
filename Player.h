#pragma once
#include "NetCodes.h"

namespace Game {
	class Player : public EngineCore::GameObject {
	private:
		EngineCore::Animation left, right, top, bottom;
		EngineCore::Animation* current = nullptr;

		bool toMove;
		bool toSend = false;
	public:
		
		Uint32 ID;
		Uint8 direction = 0;

		Player(Uint32 ID, bool toMove) : ID(ID), toMove(toMove) {}

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

			sprite.position.x = (8 + rand() % 4);
			sprite.position.y = (8 + rand() % 4);
		}

		void update() {
			current->update();

			if (toMove) {
				if (EngineCore::Keyboard::isPressed(SDLK_w)) { toSend = true; sprite.position.y += delta(0.03f); direction = 0; }
				if (EngineCore::Keyboard::isPressed(SDLK_a)) { toSend = true; sprite.position.x -= delta(0.03f); direction = 1; }
				if (EngineCore::Keyboard::isPressed(SDLK_s)) { toSend = true; sprite.position.y -= delta(0.03f); direction = 2; }
				if (EngineCore::Keyboard::isPressed(SDLK_d)) { toSend = true; sprite.position.x += delta(0.03f); direction = 3; }

				float sx = EngineCore::Window::size.x / 200,
					  sy = EngineCore::Window::size.y / 200;

				glm::vec2 cameraPosition = sprite.position - glm::vec2(
					sx-sprite.scale.x/2,
					sy+sprite.scale.y/2
				);

				if (cameraPosition.x < 0) cameraPosition.x = 0;
				if (cameraPosition.y < 0) cameraPosition.y = 0;
				if (cameraPosition.x > 8) cameraPosition.x = 8;
				if (cameraPosition.y > 10) cameraPosition.y = 10;

				if (sprite.position.x < 0)    sprite.position.x = 0;
				if (sprite.position.y < 1.35) sprite.position.y = 1.35;
				if (sprite.position.x > 15)   sprite.position.x = 15;
				if (sprite.position.y > 16)   sprite.position.y = 16;

				EngineCore::camera().position = cameraPosition;
			}

			if (direction == 0) current = &top;
			if (direction == 1) current = &left;
			if (direction == 2) current = &bottom;
			if (direction == 3) current = &right;

		}

		void netUpdate() {
			if (!toMove || !toSend) return;
			toSend = false;

			byte data[15];
			memcpy(&data[6],  &sprite.position.x, 4);
			memcpy(&data[10],  &sprite.position.y, 4);
			memcpy(&data[14], &direction, 1);

			EngineCore::Client::Send(data, NET_PLAYER_MOVE, 15);
		}
	};
}