#pragma once
#include "Log.h"

namespace EngineCore {
	struct Config {
	private:
		std::map<std::string, std::string> vars;
	public:
		static Config nil;
	};

	class ConfigReader {
	public:
		static Config read(std::string path);
	};
}