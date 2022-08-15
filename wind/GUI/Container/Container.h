#pragma once
#include "../Chat/Chat.h"

namespace WindEngine {
	class Container : public Widget {
	private:
		std::vector<Widget*> widgets;
	protected:
		void start();
		void update();
		void draw();
		void free();
	public:
		void add(Widget* widget);
		std::vector<Widget*>& getWidgets();
	};
}