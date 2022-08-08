#include "Engine.h"

namespace Game {
	void start() {
		ec::Node* gameScene = new ec::Node();
		ec::Node::root = gameScene;
		
		ec::Node2D* player = new ec::Node2D();
		player->position = { 2, 2 };
		player->size	 = { 1, 1 };
		player->addBehavior(ec::Sprite::builder("asset/sprites/sprite.meta"));

		gameScene->addChild(player);
	}

	void update() {
				
	}
}

int main(int argc, char** args) {
	ec::Core::user_start = Game::start;
	ec::Core::user_update = Game::update;

	return ec::Core::loop();
};