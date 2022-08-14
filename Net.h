#pragma once
#include "Server.h"

namespace EngineCore {
	class Net {
	private:
		static bool isServer;
	public:
		static bool init(JText::Object& config);
		static void update();
		static void free();
	};
}