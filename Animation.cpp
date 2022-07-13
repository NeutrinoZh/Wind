#include "Animation.h"

namespace EngineCore {
	void Animation::addFrame(Texture frame) {
		frames.push_back(frame);
	}

	std::vector<Texture>& Animation::get() {
		return frames;
	}
	
	void Animation::update() {
		sprite->texture = frames[current];
		if (clock() > lastTime + interval) {
			++current;

			if (current >= frames.size()) {
				++cycle;
				if (loop || cycle < cycles)
					current = 0;
				else
					current = frames.size() - 1;
			}

			lastTime = clock();
		}
	}
}