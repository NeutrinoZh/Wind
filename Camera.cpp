#include "Camera.h"

namespace EngineCore {
	Camera Camera::camera = Camera();

	Camera& camera() {
		return Camera::camera;
	}
}