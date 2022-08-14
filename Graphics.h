#pragma once
#include "Shader.h"

namespace EngineCore {
	class GL_Context {
	private:
		static SDL_GLContext context;

		static bool autoResize;
		static glm::vec2 currentSize;
	public:
		static glm::mat4 proj;
		static glm::vec3 color;
		static void (*user_render) (void);

		static bool preInit(JText::Object& obj_config);
		static bool postInit(JText::Object& obj_config);
		static void render();
		static void free();
	};
}