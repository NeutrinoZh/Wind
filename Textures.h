#pragma once
#include "Fonts.h"

namespace EngineCore {
	
	class Textures : public Resource<Texture> {
	public:
		Textures(std::string name) : Resource<Texture>(name) {};
		static Textures* textures;

		Texture createFromSurface(SDL_Surface* surface,
			Uint32 wrap, Uint32 minFilter, Uint32 magFilter,
			Uint32 internalFormat, Uint32 format, std::string name);
		Texture load(std::string);
		void free(Texture);
	};
	
	Textures& textures();
}