#pragma once
#include "Resource.h"

namespace EngineCore {
	struct Texture {
		Uint32 texture = 0;
		glm::vec4 rect = { 0, 0, 1, 1 };
		Texture() {};
		Texture(Uint32 texture, glm::vec4 rect): texture(texture), rect(rect) {};
		Texture(Uint32 texture): texture(texture) {};
	};
}