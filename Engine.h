#pragma once
#include "Scenes.h"

namespace EngineCore {
	struct Core {
	public:
		static void (*user_start) (void);
		static void (*user_update) (void);
	public:
		static int loop();
	};
}

namespace ec = EngineCore;