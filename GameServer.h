#pragma once
#include "GameClient.h"

namespace Game {
	class GameServer {
	private:
		static void addPlayer(Uint16 ID) {
			auto& players = game().players;

			if (ID >= players.size())
				players.push_back(NULL);

			players[ID] = new Player(ID, false);
			EngineCore::Core::scene->AddObject(players[ID]);
			players[ID]->Start();

			int x = 0, y = 0;
			do {
				x = rand() % game().background->tilemap.map.size();
				y = rand() % game().background->tilemap.map[0].size();
			} while (game().background->tilemap.getTile(x, y).solid ||
				game().foreground->tilemap.getTile(x, y).solid);

			game().players[ID]->sprite.position.x = -64 + x + game().players[ID]->sprite.origin.x;
			game().players[ID]->sprite.position.y = -64 + y + game().players[ID]->sprite.origin.y;
		}
	public:
		static void RequestHandler(Uint32 ID, EngineCore::Packet* packet) {
			if (packet->code == game().NET_PLAYER_MOVE) {
				Player* player = (Player*)game().players[ID - 1];

				player->sprite.position = {
					packet->read<float>(),
					packet->read<float>()
				};
				player->state = packet->read<Uint8>();

				auto clients = EngineCore::Server::getClients();
				for (Uint32 i = 1; i < clients.size(); ++i) {
					if (clients[i].ip.host == 0 || clients[i].ip.port == 0)
						continue;

					if (i != ID) {
						EngineCore::Packet packetPlayerMove = EngineCore::Packet(9);
						packetPlayerMove.code = game().NET_PLAYER_MOVE;
						packetPlayerMove.c_data = ID;
						packetPlayerMove.write<float>(player->sprite.position.x);
						packetPlayerMove.write<float>(player->sprite.position.y);
						packetPlayerMove.write<Uint8>(player->state);

						EngineCore::Server::Send(i, packetPlayerMove);
					}
				}
			}
		}

		static void ConnectHandler(Uint32 ID) {
			//===============================================================//		
			{
				EngineCore::TileMap* tilemap = NULL;

				tilemap = &game().background->tilemap;
				for (Uint32 i = 0; i < 128; ++i) {
					EngineCore::Packet line1 = EngineCore::Packet(64);
					line1.code = game().NET_MAP_GENERATE_BACKGROUND;
					for (Uint32 j = 0; j < 64; ++j)
						line1.write<Uint8>((Uint8)tilemap->map[i][j]);

					EngineCore::Packet line2 = EngineCore::Packet(64);
					line2.code = game().NET_MAP_GENERATE_BACKGROUND;
					for (Uint32 j = 64; j < 128; ++j)
						line2.write<Uint8>((Uint8)tilemap->map[i][j]);

					if (EngineCore::Server::warrantySend(ID, line1)) {
						if (!EngineCore::Server::warrantySend(ID, line2)) {
							Log::error() << "Couldn't send data of map";
							return;
						}
					} else {
						Log::error() << "Couldn't send data of map";
						return;
					}

					EngineCore::Packet okPacket = EngineCore::Packet(0);
					for (Uint32 k = 1; k < EngineCore::Server::getClients().size(); ++k)
						if (k != ID) {
							EngineCore::Server::Send(k, okPacket);

							Uint32 ready = SDLNet_SocketReady(EngineCore::Server::self.server_socket);
							if (ready > 0)
								EngineCore::Net::recieved(EngineCore::Server::self.server_socket, 128);
						}
				}

				tilemap = &game().foreground->tilemap;
				for (Uint32 i = 0; i < 128; ++i) {
					EngineCore::Packet line1 = EngineCore::Packet(64);
					line1.code = game().NET_MAP_GENERATE_FOREGROUND;
					for (Uint32 j = 0; j < 64; ++j)
						line1.write<Uint8>((Uint8)tilemap->map[i][j]);

					EngineCore::Packet line2 = EngineCore::Packet(64);
					line2.code = game().NET_MAP_GENERATE_FOREGROUND;
					for (Uint32 j = 64; j < 128; ++j)
						line2.write<Uint8>((Uint8)tilemap->map[i][j]);

					if (EngineCore::Server::warrantySend(ID, line1)) {
						if (!EngineCore::Server::warrantySend(ID, line2)) {
							Log::error() << "Couldn't send data of map";
							return;
						}
					} else {
						Log::error() << "Couldn't send data of map";
						return;
					}

					EngineCore::Packet okPacket = EngineCore::Packet(0);
					for (Uint32 k = 1; k < EngineCore::Server::getClients().size(); ++k)
						if (k != ID) {
							EngineCore::Server::Send(k, okPacket);
						
							Uint32 ready = SDLNet_SocketReady(EngineCore::Server::self.server_socket);
							if (ready > 0)
								EngineCore::Net::recieved(EngineCore::Server::self.server_socket, 128);
						}
				}
			}
			//===============================================================//
			{
				Uint16 index = ID - 1;

				addPlayer(index);

				EngineCore::Packet packetNewPlayerCreate = EngineCore::Packet(8);
				packetNewPlayerCreate.code = game().NET_PLAYER_CREATE;
				packetNewPlayerCreate.c_data = ID;
				packetNewPlayerCreate.write<float>(game().players[index]->sprite.position.x);
				packetNewPlayerCreate.write<float>(game().players[index]->sprite.position.y);

				EngineCore::Server::Send(ID, packetNewPlayerCreate);

				auto clients = EngineCore::Server::getClients();
				for (Uint32 i = 1; i < clients.size(); ++i) {
					if (clients[i].ip.host == 0 || clients[i].ip.port == 0)
						continue;

					if (i != ID) {
						packetNewPlayerCreate.packetID = clients[i - 1].lastPacketID;
						EngineCore::Server::Send(i, packetNewPlayerCreate);

						EngineCore::Packet packetOldPlayerCreate = EngineCore::Packet(8);
						packetOldPlayerCreate.code = game().NET_PLAYER_CREATE;
						packetOldPlayerCreate.c_data = i;
						packetOldPlayerCreate.write<float>(game().players[i - 1]->sprite.position.x);
						packetOldPlayerCreate.write<float>(game().players[i - 1]->sprite.position.y);

						EngineCore::Server::Send(ID, packetOldPlayerCreate);
					}
				}
			}
			//===============================================================//
		}

		static void DisconnectHandler(Uint32 ID) {
			Uint16 index = ID - 1;

			EngineCore::Core::scene->DeleteObject(game().players[index]);

			EngineCore::Packet packetDeletePlayer = EngineCore::Packet(0);
			packetDeletePlayer.code = game().NET_PLAYER_DESTROY;
			packetDeletePlayer.c_data = ID;

			auto clients = EngineCore::Server::getClients();
			for (Uint32 i = 1; i < clients.size(); ++i) {
				if (clients[i].ip.host == 0 || clients[i].ip.port == 0)
					continue;
				
				if (i != ID)
					EngineCore::Server::Send(i, packetDeletePlayer);
			}
		}
	};
}