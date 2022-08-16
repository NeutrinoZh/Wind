#include "Fonts.h"

namespace WindEngine {
	Fonts* Fonts::fonts = new Fonts("fonts");

	void Fonts::load_o(JText::Object& obj_font) {
		struct _ {
			int ptSize = 18;
			std::string path = "";
			std::string name = "";

			_(JText::Object& font) {
				name   = font["name"]._str(name);
				path   = font["path"]._str(path);
				ptSize = font["ptSize"]._int(ptSize);
			}
		} meta(obj_font);

		Log::info() << "Load ttf font:" << meta.path;
		
		TTF_Font* font = TTF_OpenFont(meta.path.c_str(), meta.ptSize);
		if (!font) {
			Log::error() << TTF_GetError();
			return;
		}

		this->add(meta.name, font);
	}

	void Fonts::free(TTF_Font* font) {
		TTF_CloseFont(font);
		font = NULL;
	}

	Fonts& fonts() {
		return *Fonts::fonts;
	}
}