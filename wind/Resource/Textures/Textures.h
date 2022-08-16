#pragma once
#include "../Fonts/Fonts.h"

namespace WindEngine {
	
	class Textures : public Resource<Texture> {
	public:
		Textures(std::string name) : Resource<Texture>(name) {};
		static Textures* textures;

		Texture createFromSurface(SDL_Surface* surface,
			Uint32 wrap, Uint32 minFilter, Uint32 magFilter,
			Uint32 internalFormat, Uint32 format, std::string name);
		void load_o(JText::Object& texture);
		void free(Texture);
	};
	
	Textures& textures();
}