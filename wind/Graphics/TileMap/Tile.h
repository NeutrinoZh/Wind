#include "../Camera/Camera.h"

namespace WindEngine {
	class Tile {
	public:
		bool solid = false;
		glm::vec4 color = { 1, 1, 1, 1 };
		Texture texture = Texture(0);

		static Tile builder(std::string path);
	};
}