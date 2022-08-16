#pragma once
#include "Scenes/Scenes.h"

namespace WindEngine {
	class Resources {
	public:
		static bool load(std::string path) {
			Log::begin() << "Start resources load:" << path;

			JText::Object resources;
			if (!JText::parse(path, resources)) {
				Log::error() << "WindEngine. Couldn't read resources file";
				return false;
			}

			nodes().loadFolder(resources["nodes"]._str(""));
			scenes().loadFolder(resources["scenes"]._str(""));
			animations().loadFolder(resources["animations"]._str(""));

			fonts().loadFromObject(resources["fonts"]);
			textures().loadFromObject(resources["textures"]);
			shaders().loadFromObject(resources["shaders"]);

			Log::end() << "Finish load resources";
			return true;
		}
	};
}