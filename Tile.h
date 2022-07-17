#include "Animator.h"

namespace EngineCore {
	class Tile {
	public:
		glm::vec4 color = { 1, 1, 1, 1 };
		Texture texture;

		static Tile builder(std::string path);
	};
}