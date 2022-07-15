#pragma once
#include "Dirt.h"

namespace Game {
	class Player : public EngineCore::GameObject {
	private:
		EngineCore::Animation left, right, top, bottom;
		EngineCore::Animation* current = nullptr;

		bool toMove;
		bool toSend = false;
	public:
		
		Uint32 ID;
		Uint8 direction;

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

			sprite.position.x = 2;
			sprite.position.y = 2;
		}

		void update() {
			current->update();

			if (toMove) {
				if (EngineCore::Keyboard::isPressed(SDLK_w)) { toSend = true; sprite.position.y += 0.03f; direction = 0; }
				if (EngineCore::Keyboard::isPressed(SDLK_a)) { toSend = true; sprite.position.x -= 0.03f; direction = 1; }
				if (EngineCore::Keyboard::isPressed(SDLK_s)) { toSend = true; sprite.position.y -= 0.03f; direction = 2; }
				if (EngineCore::Keyboard::isPressed(SDLK_d)) { toSend = true; sprite.position.x += 0.03f; direction = 3; }

				if (sprite.position.x > 8)  sprite.position.x = -1;
				if (sprite.position.y > 8)  sprite.position.y = 0;
				if (sprite.position.x < -1) sprite.position.x = 8;
				if (sprite.position.y < 0)  sprite.position.y = 8;
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
			memcpy(&data[0],  &NET_PLAYER_MOVE, 2);
			memcpy(&data[2],  &sprite.position.x, 4);
			memcpy(&data[6],  &sprite.position.y, 4);
			memcpy(&data[10], &ID, 4);
			memcpy(&data[14], &direction, 1);

			EngineCore::Client::Send(data, 15);
		}
	};
}