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

				if (game().players.size() == 1)
					game().chat->send("Welcome to the Server");
				else
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
			} else if (packet->code == game().NET_MAP_GENERATE) {
				Log::info() << "MAP GENERATE";

				EngineCore::Packet mypacket = *packet;

				Uint32 x = 0, y = 0;
				bool is = true;

				do {
					Uint32 tx = x + 64;

					if (tx >= 128) {
						x = 0;
						y += 1;
						tx = 64;
					}

					for (; x < tx; ++x) {
						Uint8 tile = mypacket.read<Uint8>();
						if (tile != 5)
							game().background->tilemap.map[x][y] = tile;
					}

					if (y != 127) {
						is = SDLNet_CheckSockets(EngineCore::Client::self.socket_set, 500) > 0;

						UDPpacket* package = EngineCore::Net::recieved(EngineCore::Client::self.client_socket, 128);
						if (!package)
							return;

						mypacket = EngineCore::Packet(package->data, package->len);

						EngineCore::Packet okPacket = EngineCore::Packet(0);
						EngineCore::Client::Send(okPacket);
					} else
						return;
				} while (is);
			}
		}
	};
}