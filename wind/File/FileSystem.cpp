#include "FileSystem.h"

namespace WindEngine {
	bool FileSystem::readFile(std::string pathToFile, std::string& out) {
		std::ifstream file(pathToFile);

		if (!file.is_open()) {
			Log::error() << "Filesystem: Failed to open file for reading:" << pathToFile;
			return false;
		}

		std::string text = "", line = "";
		while (std::getline(file, line)) {
			text += line + "\n";
		}

		file.close();
		out = text;

		return true;
	}
}