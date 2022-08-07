#pragma once
#include "Node.h"

namespace EngineCore {
	class Behavior {
	private:
		Node* node = NULL;

		friend class Node;
	public:
		virtual void start()  = 0;
		virtual void update() = 0;
		virtual void free()	  = 0;
	};
}