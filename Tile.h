#include "Camera.h"

namespace EngineCore {
	class Tile {
	public:
		bool solid = false;
		glm::vec4 color = { 1, 1, 1, 1 };
		Texture texture = Texture(0);

		static Tile builder(std::string path);
	};
}