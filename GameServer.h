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

			Packet packet = Packet::create(128 * 128 * 2, 0);
			packet.code = game().NET_MAP_GENERATE;
			
			for (Uint32 x = 0; x < 128; ++x)
				for (Uint32 y = 0; y < 128; ++y)
					packet.write<Uint8>((Uint8)game().background->tilemap.map[x][y]);

			for (Uint32 x = 0; x < 128; ++x)
				for (Uint32 y = 0; y < 128; ++y)
					packet.write<Uint8>((Uint8)game().foreground->tilemap.map[x][y]);

			Server::addToSendData(clientID, packet);

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

		static EngineCore::Packet SendPacket(Uint16 clientID) {
			using namespace EngineCore;

			static std::vector<Uint16> it;
			if (it.size() <= clientID)
				it.resize(clientID + 1);

			it[clientID] += 1;
			Uint16 i = it[clientID] % game().players.size();

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