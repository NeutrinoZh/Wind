#pragma once
#include "Node/Node.h"

namespace WindEngine {
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