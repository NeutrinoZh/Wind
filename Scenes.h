#pragma once
#include "GUIObject.h"

namespace EngineCore {
	class Scenes : public Resource<Config> {
	public:
		Scenes(std::string name) : Resource<Config>(name) {};
		static Scenes* scenes;

		Config load(std::string);
		void free(Config) {};
	};

	Scenes& scenes();
}