#pragma once
#include "Widget/Widget.h"

namespace WindEngine {
	class Widget;

	class GUI {
	public:
		static Shader* shader;
		static glm::mat4 proj;
		
		static bool init();

		static void drawRect(glm::vec2 position, glm::vec2 size, glm::vec4 color, Texture texture);
	};


}