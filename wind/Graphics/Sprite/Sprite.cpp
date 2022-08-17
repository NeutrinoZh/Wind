#include "Sprite.h"

namespace WindEngine {
	void Sprite::render() {
		if (!paint) {
			paint = static_cast<bool>(shader);
			return;
		}

		if (!shader) {
			paint = false;
			Log::error() << "When sprite drawing shader been NULL";
			return;
		}

		{
			float verticesQuad[16] = {
				0,  0,  0, 0,
				1,  0,  1, 0,
				0, -1,  0, 1,
				1, -1,  1, 1
			};

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

		Node2D* node = static_cast<Node2D*>(this->node);

		glBindTexture(GL_TEXTURE_2D, texture.texture);

		transform = glm::mat4(1);
		transform = glm::translate(transform, glm::vec3(node->position - camera().position - node->origin, 0));
		transform = glm::rotate(transform, glm::radians(node->angle), glm::vec3(0, 0, 1));
		transform = glm::scale(transform, glm::vec3(node->size, 1.0f));

		shader->use();
		shader->setMat("Proj", GL_Context::proj);
		shader->setMat("Transform", transform);
		shader->setVec4f("Color", color);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}