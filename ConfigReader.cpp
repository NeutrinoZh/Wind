#include "ConfigReader.h"

namespace EngineCore {
	const Config Config::nil = Config();

	std::string Config::getStringValue(std::string name) {
		auto it = vars.find(name);
		if (it == vars.end())
			return "";
		return it->second;
	}


	int Config::getIntValue(std::string name) {
		auto it = vars.find(name);
		if (it == vars.end())
			return 0;
		return atoi(it->second.c_str());
	}

	float Config::getFloatValue(std::string name) {
		auto it = vars.find(name);
		if (it == vars.end())
			return 0.f;
		return (float)atof(it->second.c_str());
	}

	bool Config::getBoolValue(std::string name) {
		auto it = vars.find(name);
		if (it == vars.end())
			return false;
		return it->second == "true";
	}

	bool Config::isVar(std::string name) {
		return vars.find(name) != vars.end();
	}

	void Config::addVar(std::string name, std::string value) {
		vars.insert(std::make_pair(name, value));
	}

	//=====================================================//

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
	
		Config config;

		//====================================================//

		std::string name = "";
		std::string value = "";
		bool eq = false;

		int position = 0;
		while (position < text.size()) {
			if (text[position] == '=')
				eq = true;
			else if (text[position] == '\n') {
				config.addVar(name, value);
				
				eq = false;
				name = value = "";
			} else {
				if (!eq) {
					name += text[position];
				}
				else {
					value += text[position];
				}
			}

			position++;
		}

		//==============================================//

		return config;
	}
}