#pragma once
#include "../Core/SDLSystems/SDLSystems.h"

namespace WindEngine {
	template <typename Type>
	class Resource {
	protected:
		std::map<std::string, Type> resource;
		std::string name = "resource";
	public:
		Resource(std::string name) : name(name) {};

		virtual void load_o(JText::Object& font) = 0;
		virtual void free(Type obj) = 0;

		void load_f(std::string path) {
			JText::Object resource;
			if (!JText::parse(path, resource))
				Log::error() << "WindEngine. Couldn't load " << name << " from:" << path;

			return this->load_o(resource);
		};

		void freeAll() {
			Log::info() << "Free memory from resource(" << name << ")";
			for (auto it : resource)
				free(it.second);
			resource.clear();
		}

		void loadFolder(std::string path) {
			Log::info() << "Loading resource(" << name << ") from folder:" << path;
			if (std::filesystem::is_directory(path))
				for (const auto& entry : std::filesystem::directory_iterator(path))
					load_f(entry.path().string());
			else
				Log::error() << "Failed loading resource(" << name << ") Could not find directory:" << path;
		};

		void loadFromObject(JText::Object& fonts) {
			for (auto object : fonts.children)
				load_o(*object.second);
		}

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