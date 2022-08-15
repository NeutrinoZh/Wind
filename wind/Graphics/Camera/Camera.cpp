#include "Camera.h"

namespace WindEngine {
	Camera Camera::camera = Camera();

	Camera& camera() {
		return Camera::camera;
	}
}