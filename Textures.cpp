#include "Textures.h"

namespace EngineCore {
	Textures* Textures::textures = new Textures("textures");

	Textures& textures() {
		return *Textures::textures;
	}

	Texture Textures::load(std::string path) {
		struct _meta {
			std::string path = "", name = "";
			
			Uint32 
				wrap = GL_REPEAT,
				minFilter = GL_LINEAR,
				magFilter = GL_LINEAR,
				internalFormat = GL_RGBA,
				format = GL_RGBA;

			std::vector<std::pair<std::string, glm::vec4>> atlas = {};

			_meta(std::string path) {
				Log::info() << "Load metadata for texturre:" << path;

				Config config = ConfigReader::read(path);

				if (config.isVar("wrap")) {
					std::string value = config.getStringValue("wrap");
					if      (value == "REPEAT")			 wrap = GL_REPEAT;
					else if (value == "MIRRORED_REPEAT") wrap = GL_MIRRORED_REPEAT;
					else if (value == "CLAMP_TO_EDGE")	 wrap = GL_CLAMP_TO_EDGE;
				}

				if (config.isVar("minFilter")) {
					std::string value = config.getStringValue("minFilter");
					if      (value == "LINEAR")  minFilter = GL_LINEAR;
					else if (value == "NEAREST") minFilter = GL_NEAREST;
				}

				if (config.isVar("magFilter")) {
					std::string value = config.getStringValue("magFilter");
					if      (value == "LINEAR")  magFilter = GL_LINEAR;
					else if (value == "NEAREST") magFilter = GL_NEAREST;
				}

				if (config.isVar("internalFormat")) {
					std::string value = config.getStringValue("internalFormat");
					if		(value == "RBA")  internalFormat = GL_RGB;
					else if (value == "BGR")  internalFormat = GL_BGR;
					else if (value == "RGBA") internalFormat = GL_RGBA;
					else if (value == "BGRA") internalFormat = GL_BGRA;
				}

				if (config.isVar("format")) {
					std::string value = config.getStringValue("format");
					if      (value == "RBA")  format = GL_RGB;
					else if (value == "BGR")  format = GL_BGR;
					else if (value == "RGBA") format = GL_RGBA;
					else if (value == "BGRA") format = GL_BGRA;
				}

				if (config.isVar("path")) this->path = config.getStringValue("path");
				if (config.isVar("name"))       name = config.getStringValue("name");

				for (Uint32 i = 0; i < 100; ++i) {
					std::string str = std::to_string(i);
					
					std::string name = "";
					glm::vec4 rect = { 0, 0, 1, 1 };

					if (config.isVar(str)) name = config.getStringValue(str);
					else break;

					if (config.isVar(str + "x")) rect.x = config.getFloatValue(str + "x");
					if (config.isVar(str + "y")) rect.y = config.getFloatValue(str + "y");
					if (config.isVar(str + "w")) rect.z = config.getFloatValue(str + "w");
					if (config.isVar(str + "h")) rect.w = config.getFloatValue(str + "h");

					atlas.push_back({ name, rect });
				}
			}
		} meta(path);

		Log::info() << "Load texture:" << meta.path;
		
		SDL_Surface* surface = IMG_Load(meta.path.c_str());
		if (!surface) {
			Log::error() << SDL_GetError();
			return NULL;
		}

		UINT32 texture;
		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, meta.wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, meta.wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, meta.minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, meta.magFilter);

		glTexImage2D(GL_TEXTURE_2D, 0, meta.internalFormat,
			surface->w, surface->h, 0, meta.format,
			GL_UNSIGNED_BYTE, surface->pixels);

		this->add(meta.name, Texture(texture));

		for (Uint32 i = 0; i < meta.atlas.size(); ++i) {
			Log::info() << "Create from " << meta.name << " texture: " << meta.atlas[i].first;
			this->add(meta.atlas[i].first, Texture(texture, meta.atlas[i].second));
		}

		return texture;
	}

	void Textures::free(Texture texture) {
		glDeleteTextures(1, &texture.texture);
	}
}