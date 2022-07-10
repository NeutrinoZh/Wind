#pragma once
#include "Shader.h"

namespace EngineCore {
	class GL_Context {
	private:
		static SDL_GLContext context;

		static bool autoResize;
		static glm::vec2 currentSize;
	public:
		static glm::vec3 color;
		static void (*Draw) (void);

		static bool preInit();
		static bool postInit();
		static void draw();
		static void free();
	};
}