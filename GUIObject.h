#pragma once
#include "Image.h"

namespace EngineCore {
	class GUIObject : public GameObject {
	protected:
		Container* container = NULL;

		void start();
		void update();
		void draw();
		void free();
	};
}