#include "Container.h"

namespace EngineCore {
	void Container::start() {
		for (Widget* widget : widgets)
			widget->Start();
	}

	void Container::update() {
		for (Widget* widget : widgets)
			widget->Update();
	}

	void Container::draw() {
		for (Widget* widget : widgets)
			widget->Draw();
	}

	void Container::free() {
		for (Widget* widget : widgets) {
			if (widget) {
				widget->Free();
				delete widget;
			}
		}
	}

	void Container::add(Widget* widget) {
		widgets.push_back(widget);
	}
}