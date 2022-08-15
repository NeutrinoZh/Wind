#include "../wind/Core/Engine.h"

namespace Game {
	void start() {
		wd::Node* gameScene = new wd::Node();
		wd::Node::root = gameScene;
		
		wd::Node2D* player  = new wd::Node2D();
		player->position = { 2, 2 };
		player->size	 = { 1, 1 };
		player->addBehavior(wd::Sprite::builder("asset/sprites/sprite.meta"));

		gameScene->addChild(player);
	}

	void update() {
				
	}
}

int main(int argc, char** args) {
	wd::Core::user_start = Game::start;
	wd::Core::user_update = Game::update;

	return wd::Core::loop();
};