#pragma once
#include "SDLSystems.h"

namespace EngineCore {
	template <typename Type>
	class Resource {
	protected:
		std::map<std::string, Type> resource;
		std::string name = "resource";
	public:
		Resource(std::string name) : name(name) {};

		virtual Type load(std::string path) = 0;
		virtual void free(Type obj) = 0;

		void freeAll() {
			Log::info() << "Free memory from resource(" << name << ")";
			for (auto it : resource)
				free(it.second);
			resource.clear();
		}

		void loadFolder(std::string path) {
			Log::begin() << "Start loading resource(" << name << ") from folder:" << path;
			if (std::filesystem::is_directory(path))
				for (const auto& entry : std::filesystem::directory_iterator(path))
					load(entry.path().string());
			else
				Log::error() << "Failed loading resource(" << name << ") Could not find directory:" << path;
			Log::end() << "Finish loading resource(" << name << ") from folder:" << path;
		};

		void add(std::string name, Type obj) {
			resource.insert({ name, obj });
		}
		
		std::map<std::string, Type>& get() {
			return resource;
		}

		Type operator[](std::string str) {
			return resource[str];
		}
	};
}