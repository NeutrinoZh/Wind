#include "Label.h"

namespace WindEngine {
	void Label::setText(std::string text) {
		if (this->text == text)
			return;
		this->text = text;

		if (!this->font)
			return;

		if (text.empty()) {
			this->paint = false;
			return;
		}
		this->paint = true;

		SDL_Surface* image = nullptr;
		SDL_Color color = { 255U, 255U, 255U, 255U };

		image = TTF_RenderText_Blended(this->font, text.c_str(), color);

		if (!image) {
			Log::error() << TTF_GetError();
			return;
		}

		if (texture.texture && texture.texture != textures()["none"].texture)
			textures().free(texture);
		texture = textures().createFromSurface(image, GL_REPEAT, GL_NEAREST, GL_NEAREST, GL_RGBA, GL_RGBA, "__text");

		size = {
			(float)image->w / 100,
			(float)image->h / 100,
		};

		SDL_FreeSurface(image);
	}

	void Label::draw() {
		GUI::drawRect(position, size, color, texture);
	}
}