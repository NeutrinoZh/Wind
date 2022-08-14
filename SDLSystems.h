#pragma once
#include "ConfigReader.h"

#include <SDL.h>

#include <SDL_image.h>
#include <SDL_net.h>
#include <SDL_ttf.h>

#include <glad/glad.h>
#include <SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace EngineCore {
	class SDLSystems {
	public:
		static Uint32 SDL_Flags,
					  IMG_Flags;

		static bool TTF,
					NET;

		static bool init(JText::Object& config);
		static void free();
	};
}