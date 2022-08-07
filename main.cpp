#include "Engine.h"

namespace ec = EngineCore;

namespace Game {
	void start() {
		ec::Node* gameScene = new ec::Node();
	}

	void update() {

	}

	void draw() {

	}
}

int main(int argc, char** args) {
	return ec::Core::loop();
};