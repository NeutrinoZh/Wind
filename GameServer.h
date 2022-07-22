#pragma once
#include "GameClient.h"

namespace Game {
	class GameServer {
	private:
		static Player* addPlayer(Uint16 ID) {
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

			return (Player*)players[ID];
		}
	public:
		static void ConnectHandler(Uint16 clientID) {
			using namespace EngineCore;

			{
				Player* player = addPlayer(clientID);

				Packet packet = Packet::create(11);
				packet.code = game().NET_PLAYER_CREATE;
				packet.write<Uint16>(clientID);
				packet.write<float>(player->sprite.position.x);
				packet.write<float>(player->sprite.position.y);
				packet.write<Uint8>(player->state);

				Server::addToSendToEveryone(packet);
			}

			for (Uint32 i = 0; i < game().players.size(); ++i) {
				if (!game().players[i] || i == clientID)
					continue;

				Player* player = (Player*)game().players[i];

				Packet packet = Packet::create(11);
				packet.code = game().NET_PLAYER_CREATE;
				packet.write<Uint16>(player->ID);
				packet.write<float>(player->sprite.position.x);
				packet.write<float>(player->sprite.position.y);
				packet.write<Uint8>(player->state);

				Server::addToSend(clientID, packet);
			}
		}

		static void DisconnectHandler(Uint16 clientID) {
			using namespace EngineCore;

			EngineCore::Core::scene->DeleteObject(game().players[clientID]);
			game().players[clientID] = NULL;

			Packet packet = Packet::create(2);
			packet.code = game().NET_PLAYER_DESTROY;
			packet.write<Uint16>(clientID);

			Server::addToSendToEveryone(packet);
		}

		static EngineCore::Packet SendPacket(Uint16 clientID, Uint16 packetID) {
			using namespace EngineCore;

			Uint16 i = packetID % game().players.size();

			if (i != clientID) {
				if (!game().players[i])
					return Packet::create(0);

				Player* player = (Player*)game().players[i];
				
				Packet packet = Packet::create(11);
				packet.code = game().NET_PLAYER_MOVE;
				packet.write<Uint16>(player->ID);
				packet.write<float>(player->sprite.position.x);
				packet.write<float>(player->sprite.position.y);
				packet.write<Uint8>(player->state);

				return packet;
			} else
				return Packet::create(0);
		}

		static void GetPacket(Uint16 clientID, EngineCore::Packet* packet) {
			using namespace EngineCore;

			if (packet->code == game().NET_PLAYER_MOVE) {
				Player* player = (Player*)game().players[clientID];

				player->sprite.position = {
					packet->read<float>(),
					packet->read<float>()
				};

				player->state = packet->read<Uint8>();
			}
		}
	};
}