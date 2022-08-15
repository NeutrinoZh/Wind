#include "Widget.h"

namespace WindEngine {

	namespace {
		void setAbsolutePosition(Widget* w) {
			if (w->parent)
				w->absolute = w->parent->absolute + w->position;
			else w->absolute = w->position;
		}
	}

	bool Widget::isMouseOver() {
		glm::vec2 m = Mouse::getGlobalPosition() / 100.f;
		m.y = Window::size.y / 100.f - m.y + 0.6f;
		return (m.x > absolute.x &&
			m.x <  absolute.x + size.x &&
			m.y > absolute.y &&
			m.y < absolute.y + size.y);
	}

	void Widget::Start() {
		setAbsolutePosition(this);
		start();
	}

	void Widget::Update() {
		if (!active) return;

		setAbsolutePosition(this);

		if (isMouseOver()) {
			if (Mouse::isPressed(SDL_BUTTON_LEFT)) {
				if (state == State::CLICK || state == State::PRESSED)
					state = State::PRESSED;
				else
					state = State::CLICK;
			}
			else
				state = State::OVER;
		}
		else
			state = State::NONE;

		update();
	}

	void Widget::Draw() {
		if (!active || !paint) return;

		draw();
	}

	void Widget::Free() {
		free();
	}
}