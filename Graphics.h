#pragma once
#include "Window.h"

namespace EngineCore {
	class GL_Context {
	private:
		static SDL_GLContext context;
		static glm::vec3 color;
	public:
		static void (*Draw) (void);

		static bool preInit();
		static bool postInit();
		static void draw();
		static void free();
	};
}