#pragma once
#include "JText.h"

namespace EngineCore {
	struct Config {
	private:
		std::map<std::string, std::string> vars;
	public:
		static const Config nil;

		std::string getStringValue(std::string name);
		int getIntValue(std::string name);
		float getFloatValue(std::string name);
		bool getBoolValue(std::string name);
		
		bool isVar(std::string name);

		void addVar(std::string name, std::string value);
	};

	class ConfigReader {
	public:
		static Config read(std::string path);
	};
}