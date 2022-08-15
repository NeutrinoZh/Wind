#pragma once
#include "../Button/Button.h"

namespace WindEngine {
	class Chat : public Widget {
	private:
		std::vector<WindEngine::Label*> chat = std::vector<WindEngine::Label*>();
		clock_t lastDestroy = 0;
	protected:
		void update();
		void draw();
		void free();
	public:
		TTF_Font* font = nullptr;

		void send(std::string text);
	};
}