#pragma once
#include "Server/Server.h"

namespace WindEngine {
	class Net {
	private:
		static bool isServer;
	public:
		static bool init(JText::Object& config);
		static void update();
		static void free();
	};
}