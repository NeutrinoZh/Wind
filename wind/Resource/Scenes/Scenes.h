#pragma once
#include "../../GUI/GUIObject.h"

namespace WindEngine {
	class Scenes : public Resource<JText::Object> { // !! to delete
	public:
		Scenes(std::string name) : Resource<JText::Object>(name) {};
		static Scenes* scenes;

		void load_o(JText::Object& scene);
		void free(JText::Object) {};
	};

	Scenes& scenes();
}