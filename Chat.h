#pragma once
#include "Button.h"

namespace EngineCore {
	class Chat : public Widget {
	private:
		std::vector<EngineCore::Label*> chat = std::vector<EngineCore::Label*>();
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