#include "GUI.h"

namespace WindEngine {
	Shader* GUI::shader = nullptr;
	glm::mat4 GUI::proj = glm::mat4(1);

	bool GUI::init() {
		Log::info() << "GUI-System Init";

		if (!SDLSystems::TTF) {
			Log::error() << "GUI-System requires SDL_TTF";
			return false;
		}

		Log::info() << "Create ortho-graphic matrix projection for GUI System";

		proj = glm::ortho(0.0f, Window::size.x / 100.f,
						  0.0f, Window::size.y / 100.f, -1.f, 1.0f);

		return true;
	}

	void GUI::drawRect(glm::vec2 position, glm::vec2 size, glm::vec4 color, Texture texture) {
		if (!GUI::shader) {
			Log::error() << "GUI shader is NULL";
			return;
		}

		static float verticesQuad[16] = {
			0,  0,  0, 0,
			1,  0,  1, 0,
			0, -1,  0, 1,
			1, -1,  1, 1
		};

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