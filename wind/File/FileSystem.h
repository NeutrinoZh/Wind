#pragma once
#include "../Logger/Logger.h"

namespace WindEngine {
	class FileSystem {
	public:
		static bool readFile(std::string pathToFile, std::string& out);
	};
}