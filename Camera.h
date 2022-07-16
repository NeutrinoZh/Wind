#pragma once
#include "Shaders.h"

namespace EngineCore {
	class Camera {
	public:
		glm::vec2 position = {0, 0};

		static Camera camera;
	};
	Camera& camera();
}