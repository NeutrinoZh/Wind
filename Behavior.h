#pragma once
#include "Node.h"

namespace EngineCore {
	class Behavior {
	protected:
		Node* node = NULL;

		friend class Node;
	public:
		virtual void start()  = 0;
		virtual void update() = 0;
		virtual void render() = 0;
		virtual void free()	  = 0;
	};
}