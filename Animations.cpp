#include "Animations.h"

namespace EngineCore {
	Animations* Animations::animations = new Animations("animations");

	Animation Animations::load(std::string path) {
		Log::info() << "Load animation from " << path;

		struct _meta {
			std::string name = "";
			std::vector<Texture> frames = {};
			Uint32 cycles = 1;
			clock_t interval = 100;
			bool loop = false;

			_meta(std::string path) {
				Config config = ConfigReader::read(path);

				if (config.isVar("name"))     name     = config.getStringValue("name");
				if (config.isVar("cycles"))   cycles   = config.getIntValue("cycles");
				if (config.isVar("interval")) interval = config.getIntValue("interval");
				if (config.isVar("loop"))	  loop     = config.getBoolValue("loop");

				for (Uint32 i = 0; i < 100; ++i) {
					std::string n = "frame" + std::to_string(i);
					if (config.isVar(n))
						frames.push_back(textures()[config.getStringValue(n)]);
					else break;
				}
			}
		} meta(path);

		Log::info() << "Create animation: " << meta.name;

		Animation animation;
		animation.cycles = meta.cycles;
		animation.interval = meta.interval;
		animation.loop = meta.loop;;
		for (Uint32 i = 0; i < meta.frames.size(); ++i) // ??
			animation.addFrame(meta.frames[i]);

		this->add(meta.name, animation);

		return animation;
	}

	void Animations::free(Animation animation) {};

	Animations& animations() {
		return *Animations::animations;
	}
}