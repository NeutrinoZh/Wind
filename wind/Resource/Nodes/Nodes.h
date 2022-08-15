#pragma once
#include "../../Graphics/Texture/Texture.h"

namespace WindEngine {
	class Nodes : public Resource<Config> {
	public:
		Nodes(std::string name) : Resource<Config>(name) {};
		static Nodes* nodes;

		Config load(std::string);
		void free(Config) {};
	};

	Nodes& nodes();
}