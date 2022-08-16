#pragma once
#include "../Resource/Resources.h"

namespace WindEngine {
	struct Core {
	private:
		static bool init(JText::Object& config);
		static void loadResources();
		static void free();
	public:
		static void (*user_start) (void);
		static void (*user_update) (void);
	public:
		static int loop();
	};
}

namespace wd = WindEngine;