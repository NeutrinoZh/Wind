#pragma once
#include "Behavior.h"

namespace EngineCore {
	class Node2D : public Node {
	public:
		glm::vec2 position = {};
		glm::vec2 size	   = {};
		glm::vec2 origin   = {};
		float angle		   = 0;
	};
}