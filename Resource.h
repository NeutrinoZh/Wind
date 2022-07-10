#pragma once
#include "ConfigReader.h"

#include <SDL.h>

#include <SDL_image.h>

#include <glad/glad.h>
#include <SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
			Log::info() << "Loading resource(" << name << "):" << path;
			if (std::filesystem::is_directory(path))
				for (const auto& entry : std::filesystem::directory_iterator(path))
					load(entry.path().string());
			else
				Log::error() << "Failed loading resource(" << name << ") Could not find directory:" << path;
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