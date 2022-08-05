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
		static void SetHandlers() {
			using namespace EngineCore;

			Client::addCodeHandler(game().NET_PLAYER_CREATE,  PlayerCreate);
			Client::addCodeHandler(game().NET_PLAYER_MOVE,    PlayerMove);
			Client::addCodeHandler(game().NET_PLAYER_DESTROY, PlayerDestroy);
			Client::addCodeHandler(game().NET_MAP_GENERATE,   MapGenerate);

			Client::SendPacket = SendPacket;
		}

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

		static void PlayerCreate(EngineCore::Packet* packet) {
			using namespace EngineCore;

			Uint16 id = packet->read<Uint16>();

			if (game().players.empty())
				game().playerID = id;

			if (id > game().playerID)
				game().chat->send("Player with ID:" + std::to_string(id) + " join to the server");
			else if (game().players.empty())
				game().chat->send("Welcome to the server");

			Player* player = addPlayer(id, game().players.empty());
			player->sprite.position = {
				packet->read<float>(),
				packet->read<float>()
			};
			player->state = packet->read<Uint8>();
		}
		static void PlayerMove(EngineCore::Packet* packet) {
			using namespace EngineCore;

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
		static void PlayerDestroy(EngineCore::Packet* packet) {
			using namespace EngineCore;

			Uint16 id = packet->read<Uint16>();

			Player* player = NULL;
			for (size_t i = 0; i < game().players.size(); ++i)
				if (game().players[i] && ((Player*)game().players[i])->ID == id)
					player = (Player*)game().players[i];

			if (player) {
				EngineCore::Core::scene->DeleteObject(player);
				game().chat->send("Player with ID:" + std::to_string(id) + " left the server");
			}
		}
		static void MapGenerate(EngineCore::Packet* packet) {
			using namespace EngineCore;

			for (Uint32 x = 0; x < 128; ++x)
				for (Uint32 y = 0; y < 128; ++y)
					game().background->tilemap.map[x][y] = packet->read<Uint8>();

			for (Uint32 x = 0; x < 128; ++x)
				for (Uint32 y = 0; y < 128; ++y)
					game().foreground->tilemap.map[x][y] = packet->read<Uint8>();

			saveMapToImage("./asset/map.png");
		}
	};
}