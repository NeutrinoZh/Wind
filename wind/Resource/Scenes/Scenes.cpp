#include "Scenes.h"

namespace WindEngine {
	Scenes* Scenes::scenes = new Scenes("scenes");

	void Scenes::load_o(JText::Object& obj_scene) {
		std::string name = obj_scene["name"]._str("none");

		Log::info() << "Load scene:" << name;

		this->add(name, obj_scene);
	}

	Scenes& scenes() {
		return *Scenes::scenes;
	}
}