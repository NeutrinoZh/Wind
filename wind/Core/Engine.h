#pragma once
#include "../Resource/Scenes/Scenes.h"

namespace WindEngine {
	struct Core {
	public:
		static void (*user_start) (void);
		static void (*user_update) (void);
	public:
		static int loop();
	};
}

namespace wd = WindEngine;