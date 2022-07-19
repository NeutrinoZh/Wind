#include "Settings.h"

namespace Game {
	Game* Game::game = new Game();

	Game& game() {
		return *Game::game;
	}
}