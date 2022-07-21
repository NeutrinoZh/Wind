#pragma once
#include "MapGenerate.h"

namespace Game {
	class GameClient {
	private:
		static void addPlayer(Uint32 ID, bool canMove) {
			auto& players = game().players;

			players.push_back(new Player(ID, canMove));
			EngineCore::Core::scene->AddObject(players.back());
			players.back()->Start();
		}
	public:
		static void SendData() {
			if (game().players.empty())
				return;

			Player* player = (Player*)game().players[0];

			EngineCore::Packet packet = EngineCore::Packet(9);
			packet.code = game().NET_PLAYER_MOVE;
			packet.write<float>(player->sprite.position.x);
			packet.write<float>(player->sprite.position.y);
			packet.write<Uint8>(player->state);

			EngineCore::Client::Send(packet);
		}

		static void ResponseHandler(EngineCore::Packet* packet) {
			if (packet->code == game().NET_PLAYER_CREATE) {
				Uint16 ID = packet->c_data;

				addPlayer(packet->c_data - 1, game().players.empty());

				float x = packet->read<float>(),
					  y = packet->read<float>();

				game().players.back()->sprite.position = {x, y};

				if (game().players.size() == 1) {
					game().chat->send("Welcome to the Server");
					
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
				} else
					game().chat->send("Player with ID:" + std::to_string(ID) + " join to the Server");
			} else if (packet->code == game().NET_PLAYER_MOVE) {
				Uint16 ID = packet->c_data - 1;

				for (Uint32 i = 0; i < game().players.size(); ++i) {
					Player* player = (Player*)game().players[i];
					if (player->ID == ID) {
						player->sprite.position = {
							packet->read<float>(),
							packet->read<float>()
						};
						player->state = packet->read<Uint8>();
					}
				}
			} else if (packet->code == game().NET_PLAYER_DESTROY) {
				Uint16 ID = packet->c_data - 1;

				for (Uint32 i = 0; i < game().players.size(); ++i) {
					Player* player = (Player*)game().players[i];
					if (player->ID == ID) {
						EngineCore::Core::scene->DeleteObject(game().players[i]);
						game().players.erase(game().players.begin() + i);
						game().chat->send("Player with ID: " + std::to_string(ID) + " left the server");
					}
				}
			} else if (packet->code == game().NET_MAP_GENERATE_BACKGROUND) {
				static Uint32 x = 0, y = 0;
				
				if (x == 128 && y == 128)
					x = y = 0;

				Uint32 tx = x + 64;

				if (tx > 128) {
					x = 0;
					y += 1;
					tx = 64;
				}

				for (; x < tx; ++x)
					game().background->tilemap.map[y][x] = packet->read<Uint8>();

				EngineCore::Packet okPacket = EngineCore::Packet(0);
				okPacket.code = game().NET_MAP_GENERATE_BACKGROUND;
				EngineCore::Client::Send(okPacket);
			} else if (packet->code == game().NET_MAP_GENERATE_FOREGROUND) {
				static Uint32 x = 0, y = 0;

				if (x == 128 && y == 128)
					x = y = 0;

				Uint32 tx = x + 64;

				if (tx > 128) {
					x = 0;
					y += 1;
					tx = 64;
				}

				for (; x < tx; ++x)
					game().foreground->tilemap.map[y][x] = packet->read<Uint8>();

				EngineCore::Packet okPacket = EngineCore::Packet(0);
				okPacket.code = game().NET_MAP_GENERATE_FOREGROUND;
				EngineCore::Client::Send(okPacket);
			}
		}
	};
}