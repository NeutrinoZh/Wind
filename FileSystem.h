#pragma once
#include "Log.h"

namespace EngineCore {
	class FileSystem {
	public:
		static bool readFile(std::string pathToFile, std::string& out);
	};
}