#pragma once
#include "Player.h"


namespace Game {
	void generate() {
		Uint32 w = tilemap->tilemap.map.size(),
			h = tilemap->tilemap.map[0].size();

		for (Uint32 x = 0; x < w; ++x)
			for (Uint32 y = 0; y < h; ++y)
				tilemap->tilemap.map[x][y] = 5;

		//=============================================//
		EngineCore::perlinNoiseSeed(seed);

		std::vector<std::vector<float>> temp;
		temp.resize(w);
		for (Uint32 x = 0; x < w; ++x) {
			temp[x].resize(h);
			for (Uint32 y = 0; y < h; ++y)
				temp[x][y] = EngineCore::multiPerlinNoise(x / 32.f, y / 32.f, 3, 0.8f);
		}
		//=============================================//

		// 1 - dirt
		// 2 - grass
		// 3 - sand
		// 4 - stone
		// 5 - water

		EngineCore::perlinNoiseSeed(seed - rand());
		for (Uint32 x = 0; x < w; ++x) {
			for (Uint32 y = 0; y < h; ++y) {
				float high;

				high = std::sin(((float)(x + 40) / (w / 2))) * std::sin(((float)(y + 40) / (h / 2)));
				high *= EngineCore::multiPerlinNoise(x / 32.f, y / 32.f, 1, 0.8f);

				if (high > 0.08) {
					if (temp[x][y] > 0.05) tilemap->tilemap.map[x][y] = 1;
					else				   tilemap->tilemap.map[x][y] = 2;

					if (high > 0.3) tilemap->tilemap.map[x][y] = 4;
				}
				else if (high > 0.02) tilemap->tilemap.map[x][y] = 3;
			}
		}
	}

	namespace Online {
		std::vector<Player*> players;

		void ResponseHandler(Uint16 code, byte* data, Uint32 len) {
			Uint32 ID;
			memcpy(&ID, &data[2], 4);

			if (code == NET_PLAYER_MOVE) {
				float x, y;
				Uint8 state;

				memcpy(&x, &data[6], 4);
				memcpy(&y, &data[10], 4);
				memcpy(&state, &data[14], 1);

				for (Player* player : players)
					if (player->ID == ID) {
						player->sprite.position.x = x;
						player->sprite.position.y = y;
						player->state = state;
					}
			} else if (code == NET_PLAYER_CREATE) {
				for (Player* player : players)
					if (player->ID == ID)
						return;

				float x, y;
				memcpy(&x, &data[6], 4);
				memcpy(&y, &data[10], 4);

				players.push_back(new Player(ID, players.size() == 0));
				EngineCore::Core::scene->AddObject(players.back());
				players.back()->Start();

				players.back()->sprite.position.x = x;
				players.back()->sprite.position.y = y;
			} else if (code == NET_PLAYER_DESTROY) {
				for (Uint32 i = 0; i < players.size(); ++i)
					if (players[i]->ID == ID) {
						EngineCore::Core::scene->DeleteObject(players[i]);
						players.erase(players.begin() + i);
					}
			} else if (code == NET_MAP_GENERATE) {
				memcpy(&seed, &data[2], 4);
				generate();
			}
		}

		void ConnectHandler(Uint32 ID) {
			if (ID >= players.size())
				players.push_back(NULL);

			players[ID] = new Player(ID, false);
			EngineCore::Core::scene->AddObject(players[ID]);
			players[ID]->Start();

			players[ID]->sprite.position.x = (-5 + rand() % 10);
			players[ID]->sprite.position.y = (-5 + rand() % 10);

			byte data1[14];
			memcpy(&data1[2], &ID, 4);
			memcpy(&data1[6], &players[ID]->sprite.position.x, 4);
			memcpy(&data1[10], &players[ID]->sprite.position.y, 4);

			EngineCore::Server::Send(ID, NET_PLAYER_CREATE, data1, 14);

			byte data2[14];

			std::vector<IPaddress> clients = EngineCore::Server::getClients();
			for (Uint32 i = 0; i < clients.size(); ++i) {
				if (clients[i].port == NULL) continue;

				if (i != ID) {
					EngineCore::Server::Send(i, NET_PLAYER_CREATE, data1, 14);

					memcpy(&data2[2], &i, 4);
					memcpy(&data2[6], &players[i]->sprite.position.x, 4);
					memcpy(&data2[10], &players[i]->sprite.position.y, 4);

					EngineCore::Server::Send(ID, NET_PLAYER_CREATE, data2, 14);
				}
			}

			byte data3[6];
			memcpy(&data3[2], &seed, 4);
			EngineCore::Server::Send(ID, NET_MAP_GENERATE, data3, 6);
		}

		void DisconnectHandler(Uint32 ID) {
			EngineCore::Core::scene->DeleteObject(players[ID]);

			byte data[6];
			memcpy(&data[2], &ID, 4);

			std::vector<IPaddress> clients = EngineCore::Server::getClients();
			for (Uint32 i = 0; i < clients.size(); ++i) {
				if (clients[i].port == NULL) continue;
				
				if (i != ID)
					EngineCore::Server::Send(i, NET_PLAYER_DESTROY, data, 6);
			}
		}

		void RequestHandler(Uint16 code, Uint32 ID, byte* data, Uint32 len) {
			if (code == NET_PLAYER_MOVE) {
				float x, y;
				Uint8 state;

				memcpy(&x, &data[6], 4);
				memcpy(&y, &data[10], 4);
				memcpy(&state, &data[14], 1);

				players[ID]->sprite.position.x = x;
				players[ID]->sprite.position.y = y;
				players[ID]->state = state;

				std::vector<IPaddress> clients = EngineCore::Server::getClients();
				for (Uint32 i = 0; i < clients.size(); ++i) {
					if (clients[i].port == NULL) continue;
					if (i == ID) continue;

					byte data[15];

					memcpy(&data[2], &ID, 4);
					memcpy(&data[6], &players[ID]->sprite.position.x, 4);
					memcpy(&data[10], &players[ID]->sprite.position.y, 4);
					memcpy(&data[14], &players[ID]->state, 1);

					EngineCore::Server::Send(i, NET_PLAYER_MOVE, data, 15);
				}
			}
		}
	}
}