#pragma once
#include "Sprite.h"

namespace EngineCore {
	class Animation {
	private:
		std::vector<Texture> frames = {};
		size_t current = 0;

		clock_t lastTime = 0;
		size_t cycle = 0;
	public:
		Sprite* sprite = nullptr;

		clock_t interval = 100;
		size_t cycles = 1;
		bool loop = false;

		void addFrame(Texture frame);

		std::vector<Texture>& get();

		void update();
	};
}