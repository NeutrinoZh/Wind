#include "Fonts.h"

namespace EngineCore {
	Fonts* Fonts::fonts = new Fonts("fonts");

	TTF_Font* Fonts::load(std::string path) {
		struct _meta {
			int ptsize = 18;
			std::string path = "";
			std::string name = "";

			_meta(std::string path) {
				Log::info() << "Load metadata for font:" << path;

				Config config = ConfigReader::read(path);

				if (config.isVar("name"))   this->name = config.getStringValue("name");
				if (config.isVar("path"))   this->path   = config.getStringValue("path");
				if (config.isVar("ptSize")) this->ptsize = config.getIntValue("ptSize");
			}
		} meta(path);

		Log::info() << "Load ttf font:" << meta.path;
		
		TTF_Font* font = TTF_OpenFont(meta.path.c_str(), meta.ptsize);
		if (!font) {
			Log::error() << TTF_GetError();
			return nullptr;
		}

		this->add(meta.name, font);

		return font;
	}

	void Fonts::free(TTF_Font* font) {
		delete font;
	}

	Fonts& fonts() {
		return *Fonts::fonts;
	}
}