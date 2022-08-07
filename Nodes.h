#pragma once
#include "Texture.h"

namespace EngineCore {
	class Nodes : public Resource<Config> {
	public:
		Nodes(std::string name) : Resource<Config>(name) {};
		static Nodes* nodes;

		Config load(std::string);
		void free(Config) {};
	};

	Nodes& nodes();
}