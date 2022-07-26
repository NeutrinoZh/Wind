#pragma once
#include "Container.h"

namespace EngineCore {
	class Image : public Widget {
	protected:
		void start();
		void draw();
	public:
		Image() {};
		Image(Texture texture, glm::vec4 color = { 1, 1, 1, 1 }): texture(texture), color(color) {};

		Texture texture = Texture(0);
		glm::vec4 color = { 1, 1, 1, 1 };
	};
}