#pragma once
#include "FPSCounter/FPSCounter.h"

namespace WindEngine {
	struct Core {
	private:
		static bool init(JText::Object& config);
		static void loadResources();
		static void free();
	public:
		static bool activeLoop;
		static void (*user_start) (void);
		static void (*user_update) (void);
		static bool (*user_exit) (void);
	public:
		static void quit();
		static int loop();
	};
}

namespace wd = WindEngine;