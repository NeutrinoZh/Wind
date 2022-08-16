#include "Textures.h"

namespace WindEngine {
	Textures* Textures::textures = new Textures("textures");

	Textures& textures() {
		return *Textures::textures;
	}
	
	Texture Textures::createFromSurface(SDL_Surface* surface,
										Uint32 wrap, Uint32 minFilter, Uint32 magFilter, 
										Uint32 internalFormat, Uint32 format, std::string name) {
		UINT32 texture;
		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			surface->w, surface->h, 0, format,
			GL_UNSIGNED_BYTE, surface->pixels);

		Texture result = Texture(texture);
		this->add(name, result);

		return result;
	}

	void Textures::load_o(JText::Object& obj_texture) {
		struct _ {
			std::string path = "", name = "";
			
			Uint32 
				wrap = GL_REPEAT,
				minFilter = GL_LINEAR,
				magFilter = GL_LINEAR,
				internalFormat = GL_RGBA,
				format = GL_RGBA;

			std::vector<std::pair<std::string, glm::vec4>> atlas = {};

			_(JText::Object& texture) {
				std::string wrap = texture["wrap"]._str("");
				if      (wrap == "REPEAT")			this->wrap = GL_REPEAT;
				else if (wrap == "MIRRORED_REPEAT") this->wrap = GL_MIRRORED_REPEAT;
				else if (wrap == "CLAMP_TO_EDGE")	this->wrap = GL_CLAMP_TO_EDGE;

				
				std::string minFilter = texture["minFilter"]._str("");
				if      (minFilter == "LINEAR")  this->minFilter = GL_LINEAR;
				else if (minFilter == "NEAREST") this->minFilter = GL_NEAREST;

				std::string magFilter = texture["magFilter"]._str("");
				if      (magFilter == "LINEAR")  this->magFilter = GL_LINEAR;
				else if (magFilter == "NEAREST") this->magFilter = GL_NEAREST;
				
				std::string internalFormat = texture["internalFormat"]._str("");
				if		(internalFormat == "RBA")  this->internalFormat = GL_RGB;
				else if (internalFormat == "BGR")  this->internalFormat = GL_BGR;
				else if (internalFormat == "RGBA") this->internalFormat = GL_RGBA;
				else if (internalFormat == "BGRA") this->internalFormat = GL_BGRA;

				
				std::string format = texture["format"]._str("");
				if      (format == "RBA")  this->format = GL_RGB;
				else if (format == "BGR")  this->format = GL_BGR;
				else if (format == "RGBA") this->format = GL_RGBA;
				else if (format == "BGRA") this->format = GL_BGRA;

				path = texture["path"]._str(path);
				name = texture["name"]._str(name);

				Uint32 x = texture["gridX"]._uint32(0),
					   y = texture["gridY"]._uint32(0);

				if (x + y != 0) {
					Uint32 size = x * y;
					
					float stepX = 1.f / x,
						  stepY = 1.f / y;

					for (Uint32 i = 0; i < x * y; ++i) {
						if (i >= texture["textures"].children.size())
							break;

						std::string name = texture["textures"][i]._str("");
						glm::vec4 rect = {
							(i % x) * stepX,
							(i / x) * stepY,
							(i % x) * stepX + stepX,
							(i / x) * stepY + stepY
						};

						atlas.push_back({ name, rect });
					}
				}
			}
		} meta(obj_texture);

		Log::info() << "Load texture:" << meta.path;
		
		SDL_Surface* surface = IMG_Load(meta.path.c_str());
		if (!surface) {
			Log::error() << SDL_GetError();
			return;
		}

		Texture texture = createFromSurface(surface,
											meta.wrap, meta.minFilter, meta.magFilter,
											meta.internalFormat, meta.format, meta.name);

		for (Uint32 i = 0; i < meta.atlas.size(); ++i) {
			Log::info() << "Create from " << meta.name << " texture: " << meta.atlas[i].first;
			this->add(meta.atlas[i].first, Texture(texture.texture, meta.atlas[i].second));
		}
	}

	void Textures::free(Texture texture) {
		glDeleteTextures(1, &texture.texture);
	}
}