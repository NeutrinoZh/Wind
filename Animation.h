#pragma once
#include "Sprite.h"

namespace EngineCore {
	class Animation {
	private:
		std::vector<Texture> frames = {};
		Uint32 current = 0;

		clock_t lastTime = 0;
		Uint32 cycle = 0;
	public:
		Sprite* sprite = nullptr;

		clock_t interval = 100;
		Uint32 cycles = 1;
		bool loop = false;

		void addFrame(Texture frame);

		std::vector<Texture>& get();

		void update();
	};
}