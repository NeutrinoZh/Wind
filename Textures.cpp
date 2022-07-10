#include "Textures.h"

namespace EngineCore {
	Textures* Textures::textures = new Textures("textures");

	Textures& textures() {
		return *Textures::textures;
	}

	UINT32 Textures::load(std::string path) {
		Log::info() << "Load texture:" << path;
		
		SDL_Surface* surface = IMG_Load(path.c_str());
		if (!surface) {
			Log::error() << SDL_GetError();
			return NULL;
		}

		UINT32 texture;
		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA,
			surface->w, surface->h, 0, GL_BGRA,
			GL_UNSIGNED_BYTE, surface->pixels);

		return texture;
	}

	void Textures::free(UINT32 texture) {
		glDeleteTextures(1, &texture);
	}
}