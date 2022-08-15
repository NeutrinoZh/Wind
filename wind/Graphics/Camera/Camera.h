#pragma once
#include "../../Resource/Shaders/Shaders.h"

namespace WindEngine {
	class Camera {
	public:
		glm::vec2 position = {0, 0};

		static Camera camera;
	};
	Camera& camera();
}