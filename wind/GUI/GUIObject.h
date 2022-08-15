#pragma once
#include "Image/Image.h"

namespace WindEngine {
	class GUIObject : public Node {
	protected:
		Container* container = NULL;

		void start();
		void update();
		void draw();
		void free();
	};
}