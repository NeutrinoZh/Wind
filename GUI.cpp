#include "GUI.h"

namespace EngineCore {
	Shader* GUI::shader = nullptr;
	glm::mat4 GUI::proj = glm::mat4(1);

	void GUI::init() {
		Log::begin() << "Init GUI System";

		Log::info() << "Init SDL ttf";
		if (TTF_Init() != 0) {
			Log::error() << TTF_GetError();
			return;
		}

		Log::info() << "Create ortho-graphic matrix projection for GUI System";

		proj = glm::ortho(0.0f, Window::size.x / 100.f,
						  0.0f, Window::size.y / 100.f, -1.f, 1.0f);

		Log::end() << "Success init GUI System";
	}

	void GUI::free() {
		Log::info() << "Free memory from GUI System";

		TTF_Quit();
	}

	//====================================================================//

	namespace {
		float verticesQuad[16] = {
			0,  0,  0, 0,
			1,  0,  1, 0,
			0, -1,  0, 1,
			1, -1,  1, 1
		};

		void drawRect(glm::vec2 position, glm::vec2 size, glm::vec4 color, Texture texture) {
			{
				verticesQuad[2] = texture.rect.x;
				verticesQuad[6] = texture.rect.z;
				verticesQuad[10] = texture.rect.x;
				verticesQuad[14] = texture.rect.z;

				verticesQuad[3] = texture.rect.y;
				verticesQuad[7] = texture.rect.y;
				verticesQuad[11] = texture.rect.w;
				verticesQuad[15] = texture.rect.w;

				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, verticesQuad, GL_DYNAMIC_DRAW);
			}

			glBindTexture(GL_TEXTURE_2D, texture.texture);

			glm::mat4 transform = glm::mat4(1);
			transform = glm::translate(transform, glm::vec3(position, 0));
			transform = glm::scale(transform, glm::vec3(size, 1.0f));

			GUI::shader->use();
			GUI::shader->setMat("Proj", GUI::proj);
			GUI::shader->setMat("Transform", transform);
			GUI::shader->setVec4f("Color", color);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	//====================================================================//

	namespace {
		void setAbsolutePosition(Widget* w) {
			if (w->parent)
				w->absolute = w->parent->absolute + w->position;
			else w->absolute = w->position;
		}
	}

	bool Widget::isMouseOver() {
		glm::vec2 m = Mouse::getGlobalPosition() / 100.f;
		m.y = Window::size.y / 100.f - m.y + 1;
		return (m.x >  absolute.x &&
				m.x <  absolute.x + size.x &&
				m.y > absolute.y &&
				m.y <  absolute.y + size.y);
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
			} else
				state = State::OVER;
		} else
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


	//======================================================================//

	void Button::start() {
		texture = textures()["none"];
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
	}

	void Button::draw() {
		drawRect(position, size, color, texture);
	}

	//==================================================================//

	void Label::setText(std::string text) {
		if (this->text == text)
			return;
		this->text = text;

		if (!this->font)
			return;

		if (text.empty()) {
			this->paint = false;
			return;
		}
		this->paint = true;

		SDL_Surface* image = nullptr;
		SDL_Color color = { 255U, 255U, 255U, 255U };

		image = TTF_RenderText_Blended(this->font, text.c_str(), color);

		if (!image) {
			Log::error() << TTF_GetError();
			return;
		}

		if (texture.texture && texture.texture != textures()["none"].texture)
			textures().free(texture);
		texture = textures().createFromSurface(image, GL_REPEAT, GL_NEAREST, GL_NEAREST, GL_RGBA, GL_RGBA, "__text");

		size = {
			(float)image->w / 100,
			(float)image->h / 100,
		};

		SDL_FreeSurface(image);
	}

	void Label::draw() {
		drawRect(position, size, color, texture);
	}
}