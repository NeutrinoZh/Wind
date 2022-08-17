#pragma once
#include "EventHandler.h"

namespace WindEngine {
	class Window {
	public:;
		static glm::vec2 size;

		static SDL_Window* window;
		static SDL_Event event;

		static bool init(JText::Object& config);
		static void eventHandlers();
		static void free();
	};
}