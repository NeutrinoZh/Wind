#include "Scenes.h"

namespace WindEngine {
	Scenes* Scenes::scenes = new Scenes("scenes");

	Config Scenes::load(std::string path) {
		Log::info() << "Load scene:" << path;

		Config node = ConfigReader::read(path);

		if (node.isVar("name"))
			this->add(node.getStringValue("name"), node);
		else
			Log::error() << "Couldn't load scene: missing scene name";

		return node;
	}

	Scenes& scenes() {
		return *Scenes::scenes;
	}
}