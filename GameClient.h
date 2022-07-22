#pragma once
#include "MapGenerate.h"

namespace Game {
	class GameClient {
	private:
		static Player* addPlayer(Uint32 ID, bool canMove) {
			auto& players = game().players;

			players.push_back(new Player(ID, canMove));
			EngineCore::Core::scene->AddObject(players.back());
			players.back()->Start();

			return (Player*)players.back();
		}
	public:
		static EngineCore::Packet SendPacket() {
			using namespace EngineCore;

			if (game().players.empty())
				return Packet::create(0);

			
			Player* player = (Player*)game().players[0];

			Packet packet = Packet::create(9);
			packet.code = game().NET_PLAYER_MOVE;
			packet.write<float>(player->sprite.position.x);
			packet.write<float>(player->sprite.position.y);
			packet.write<Uint8>(player->state);

			return packet;
		}

		static void GetPacket(EngineCore::Packet* packet) {
			using namespace EngineCore;

			if (packet->code == game().NET_PLAYER_CREATE) {
				Uint16 id = packet->read<Uint16>();

				Player* player = addPlayer(id, game().players.empty());
				player->sprite.position = {
					packet->read<float>(),
					packet->read<float>()
				};
				player->state = packet->read<Uint8>();
			} else if (packet->code == game().NET_PLAYER_MOVE) {
				Uint16 id = packet->read<Uint16>();
				
				Player* player = NULL;
				for (size_t i = 0; i < game().players.size(); ++i)
					if (game().players[i] && ((Player*)game().players[i])->ID == id)
						player = (Player*)game().players[i];

				if (player) {
					player->sprite.position = {
						packet->read<float>(),
						packet->read<float>()
					};
					player->state = packet->read<Uint8>();
				}
			}
		}
	};
}