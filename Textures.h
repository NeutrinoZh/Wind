#pragma once
#include "Texture.h"

namespace EngineCore {
	
	class Textures : public Resource<Texture> {
	public:
		Textures(std::string name) : Resource<Texture>(name) {};
		static Textures* textures;

		Texture load(std::string);
		void free(Texture);
	};
	
	Textures& textures();
}