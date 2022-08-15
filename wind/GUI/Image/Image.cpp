#include "Image.h"

namespace WindEngine {
	void Image::start() {
		if (texture.texture == NULL)
			texture = textures()["none"];
	}

	void Image::draw() {
		GUI::drawRect(position, size, color, texture);
	}
}