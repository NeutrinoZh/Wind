#include "Player.h"

namespace Game {
	void start() {
		EngineCore::scene->AddObject(new Dirt());
		EngineCore::scene->AddObject(new Player());
	}
}

int main(int argc, char** args) {
	EngineCore::Start = Game::start;
	return EngineCore::loop();
};