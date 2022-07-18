#pragma once
#include "Engine.h"
namespace Game {
	Uint16 NET_PLAYER_MOVE = 0;
	Uint16 NET_PLAYER_CREATE = 1;
	Uint16 NET_PLAYER_DESTROY = 2;
	Uint16 NET_MAP_GENERATE = 3;

	Uint32 seed = 0;
	EngineCore::TileMapObject* background;
	EngineCore::TileMapObject* foreground;
}