#pragma once
#include "../Label/Label.h"

namespace WindEngine {
	class Button : public Widget {
	private:
		Label* label = NULL;

		Texture texture = Texture(0);
		glm::vec4 color = { 1, 1, 1, 1 };
	protected:
		void start();
		void update();
		void draw();
	public:
		std::string text = "";

		TTF_Font* font = NULL;

		Texture t_none = Texture(0);
		Texture t_over = Texture(0);
		Texture t_pressed = Texture(0);

		glm::vec4 c_text = { 0.f, 0.f, 0.f, 1.f };
		glm::vec4 c_none = { 0.9f, 0.9f, 0.9f, 1.f };
		glm::vec4 c_over = { 1.f , 1.f , 1.f , 1.f };
		glm::vec4 c_pressed = { 0.8f, 0.8f, 0.8f, 1.f };
	};
}