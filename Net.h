#pragma once
#include "Server.h"

namespace EngineCore {
	class Net {
	public:
		static bool isServer;
		static bool init();
		static void update();
		static void free();
	};
}