#pragma once
#include "Player.h"

namespace Game {
	namespace Online {
		std::vector<Player*> players;

		void ResponseHandler(byte* data, Uint32 len) {
			Uint16 code;
			memcpy(&code, &data[0], 2);

			Uint32 ID;
			memcpy(&ID, &data[2], 4);

			if (code == NET_PLAYER_MOVE) {
				float x, y;
				Uint8 direction;

				memcpy(&x, &data[6], 4);
				memcpy(&y, &data[10], 4);
				memcpy(&direction, &data[14], 1);

				if (ID < players.size()) {
					for (Player* player : players)
						if (player->ID == ID) {
							player->sprite.position.x = x;
							player->sprite.position.y = y;
							player->direction = direction;
						}
				}
			}
			else if (code == NET_CREATE_PLAYER) {
				players.push_back(new Player(ID, players.size() == 0));
				EngineCore::scene->AddObject(players.back());
				players.back()->Start();
			}
		}

		void ConnectHandler(Uint32 id) {
			Uint32 ID = players.size();

			players.push_back(new Player(ID, false));
			EngineCore::scene->AddObject(players.back());
			players.back()->Start();

			byte data1[6];
			memcpy(&data1[0], &NET_CREATE_PLAYER, 2);
			memcpy(&data1[2], &ID, 4);

			EngineCore::Server::Send(ID, data1, 6);

			byte data2[6];
			memcpy(&data2[0], &NET_CREATE_PLAYER, 2);

			for (Uint32 i = 0; i < players.size(); ++i) {
				if (i != ID) {
					EngineCore::Server::Send(i, data1, 6); // !!

					memcpy(&data2[2], &i, 4);
					EngineCore::Server::Send(ID, data2, 6);
				}
			}
		}

		void RequestHandler(Uint16 code, byte* data, Uint32 len) {
			if (code == NET_PLAYER_MOVE) {
				float x, y;
				Uint32 ID;
				Uint8 direction;

				memcpy(&x, &data[2], 4);
				memcpy(&y, &data[6], 4);
				memcpy(&ID, &data[10], 4);
				memcpy(&direction, &data[14], 1);

				players[ID]->sprite.position.x = x;
				players[ID]->sprite.position.y = y;
				players[ID]->direction = direction;

				for (Uint32 i = 0; i < players.size(); ++i) {
					byte data[15];

					memcpy(&data[0], &NET_PLAYER_MOVE, 2);
					memcpy(&data[2], &ID, 4);
					memcpy(&data[6], &players[ID]->sprite.position.x, 4);
					memcpy(&data[10], &players[ID]->sprite.position.y, 4);
					memcpy(&data[14], &players[ID]->direction, 1);

					EngineCore::Server::Send(i, data, 15); // !!
				}
			}
		}
	}
}