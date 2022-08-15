#pragma once
#include "../../Resource/Animations/Animations.h"

namespace WindEngine {
	class Animator {
	public:
		std::map<std::string, Animation> states;
		Animation* current = nullptr;
		Sprite* sprite = nullptr;

		void add(std::string name) {
			if (sprite) {
				Animation animation = animations()[name];
				animation.sprite = sprite;

				states.insert(std::make_pair(name, animation));
			} else
				Log::warning() << "Animator. Missing sprite because ignore add animation: " << name;
		}

		void set(std::string name) {
			auto animation = states.find(name);
			if (animation != states.end())
				current = &animation->second;
			else {
				current = nullptr;
				sprite->texture = Texture(0);
			}
		}

		void update() {
			if (current)
				current->update();
		}
	};
}