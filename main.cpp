#include "Player.h"

namespace Game {
	EngineCore::GameObject* player;
	
	void start() {
		EngineCore::scene->AddObject(new Dirt());

		player = new Player();
		EngineCore::scene->AddObject(player);
	}


	void update() {

	}
}

int main(int argc, char** args) {
	EngineCore::Start = Game::start;
	EngineCore::Update = Game::update;
	return EngineCore::loop();
};