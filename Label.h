#pragma once
#include "GUI.h"

namespace EngineCore {
	class Label : public Widget {
	private:
		Texture texture = Texture(0);
		std::string text = "";
	public:
		glm::vec4 color = { 1, 1, 1, 1 };
		TTF_Font* font = nullptr;

		void setText(std::string text);
	protected:
		void draw();
	};
}