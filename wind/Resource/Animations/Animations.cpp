#include "Animations.h"

namespace WindEngine {
	Animations* Animations::animations = new Animations("animations");

	void Animations::load_o(JText::Object& obj_animation) {
		struct _ {
			std::string name = "";
			Uint32 cycles = 1;
			clock_t interval = 100;
			bool loop = false;

			std::vector<Texture> frames = {};

			_(JText::Object& animation) {
				name = animation["name"]._str(name);
				cycles = animation["cycles"]._uint32(cycles);
				interval = animation["interval"]._uint32(interval);
				loop = animation["loop"]._bool(loop);

				for (auto frame : animation["frames"].children)
					frames.push_back(textures()[frame.second->_str("none")]);
			}
		} meta(obj_animation);

		Log::info() << "Create animation: " << meta.name;

		Animation animation;
		animation.cycles = meta.cycles;
		animation.interval = meta.interval;
		animation.loop = meta.loop;;
		animation.get() = meta.frames;

		this->add(meta.name, animation);
	}

	void Animations::free(Animation animation) {};

	Animations& animations() {
		return *Animations::animations;
	}
}