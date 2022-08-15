#pragma once
#include "../Log/Log.h"

namespace WindEngine {
	class FileSystem {
	public:
		static bool readFile(std::string pathToFile, std::string& out);
	};
}