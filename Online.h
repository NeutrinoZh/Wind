#pragma once
#include "Player.h"

namespace Game {
	namespace Online {
		std::vector<Player*> players;

		void ResponseHandler(Uint16 code, byte* data, Uint32 len) {
			Uint32 ID;
			memcpy(&ID, &data[2], 4);

			if (code == NET_PLAYER_MOVE) {
				float x, y;
				Uint8 direction;

				memcpy(&x, &data[6], 4);
				memcpy(&y, &data[10], 4);
				memcpy(&direction, &data[14], 1);

				for (Player* player : players)
					if (player->ID == ID) {
						player->sprite.position.x = x;
						player->sprite.position.y = y;
						player->direction = direction;
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

				srand(seed);
				for (Uint32 x = 0; x < 16; ++x)
					for (Uint32 y = 0; y < 16; ++y)
						tilemap->tilemap.map[x][y] = (rand() % 10 > 7) ? 1 : 2;
			}
		}

		void ConnectHandler(Uint32 ID) {
			if (ID >= players.size())
				players.push_back(NULL);

			players[ID] = new Player(ID, false);
			EngineCore::Core::scene->AddObject(players[ID]);
			players[ID]->Start();

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
				Uint8 direction;

				memcpy(&x, &data[6], 4);
				memcpy(&y, &data[10], 4);
				memcpy(&direction, &data[14], 1);

				players[ID]->sprite.position.x = x;
				players[ID]->sprite.position.y = y;
				players[ID]->direction = direction;

				std::vector<IPaddress> clients = EngineCore::Server::getClients();
				for (Uint32 i = 0; i < clients.size(); ++i) {
					if (clients[i].port == NULL) continue;

					byte data[15];

					memcpy(&data[2], &ID, 4);
					memcpy(&data[6], &players[ID]->sprite.position.x, 4);
					memcpy(&data[10], &players[ID]->sprite.position.y, 4);
					memcpy(&data[14], &players[ID]->direction, 1);

					EngineCore::Server::Send(i, NET_PLAYER_MOVE, data, 15);
				}
			}
		}
	}
}