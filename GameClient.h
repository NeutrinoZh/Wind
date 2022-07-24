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
			} else if (packet->code == game().NET_MAP_GENERATE) {
				for (Uint32 x = 0; x < 128; ++x)
					for (Uint32 y = 0; y < 128; ++y)
						game().background->tilemap.map[x][y] = packet->read<Uint8>();

				for (Uint32 x = 0; x < 128; ++x)
					for (Uint32 y = 0; y < 128; ++y)
						game().foreground->tilemap.map[x][y] = packet->read<Uint8>();

				SDL_Surface* surface = SDL_CreateRGBSurface(NULL, 128, 128, 32, 0, 0, 0, 0);
				Uint32* pixels = (Uint32*)surface->pixels;

				for (Uint32 x = 0; x < 128; ++x)
					for (Uint32 y = 0; y < 128; ++y) {
						Uint32 color = 0;

						if (game().background->tilemap.map[x][y] == 5) color = SDL_MapRGBA(surface->format, 0, 0, 255, 255);
						if (game().background->tilemap.map[x][y] == 4) color = SDL_MapRGBA(surface->format, 132, 132, 132, 255);
						if (game().background->tilemap.map[x][y] == 3) color = SDL_MapRGBA(surface->format, 248, 235, 0, 255);
						if (game().background->tilemap.map[x][y] == 2) color = SDL_MapRGBA(surface->format, 0, 255, 0, 255);
						if (game().background->tilemap.map[x][y] == 1) color = SDL_MapRGBA(surface->format, 113, 42, 30, 255);

						pixels[x + (128 * y)] = color;
					}

				SDL_SaveBMP(surface, "./asset/map.png");
			}
		}
	};
}