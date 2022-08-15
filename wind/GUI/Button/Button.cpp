#include "Button.h"

namespace WindEngine {
	void Button::start() {
		texture = textures()["none"];

		label = new Label();
		label->font = font;
		label->color = c_text;

		label->position = position;
		label->Start();
	}

	void Button::update() {
		if (state == State::OVER) {
			if (t_over.texture)
				texture = t_over;
			color = c_over;
		} else if (state == State::PRESSED || state == State::CLICK) {
			if (t_pressed.texture)
				texture = t_pressed;
			color = c_pressed;
		} else {
			if (t_none.texture)
				texture = t_none;
			else
				texture = textures()["none"];
			color = c_none;
		}

		label->position = position + glm::vec2((size.x / 2 - label->size.x / 2), -(size.y / 2 - label->size.y / 2));
		label->setText(text);
		label->Update();
	}

	void Button::draw() {
		GUI::drawRect(position, size, color, texture);
		label->Draw();
	}
}