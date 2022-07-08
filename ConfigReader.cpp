#include "ConfigReader.h"

namespace EngineCore {
	Config ConfigReader::read(std::string path) {

		std::string text = "";

		//====================================================//

		std::ifstream file(path);

		if (!file.is_open()) {
			Log::error() << "Couldn't open config file:" << path;
			return Config::nil;
		}

		std::string line = "";
		while (std::getline(file, line)) {
			text += line + "\n";
		}

		file.close();

		//====================================================//
	
		
	}
}