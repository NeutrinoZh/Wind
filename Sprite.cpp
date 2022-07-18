#include "Sprite.h"

namespace EngineCore {
	Sprite Sprite::builder(std::string path) {
		Sprite sprite;

		Config config = ConfigReader::read(path);
		
		if (config.isVar("angle")) sprite.angle = config.getFloatValue("angle");
		if (config.isVar("texture"))
			sprite.texture = textures()[config.getStringValue("texture")];

		if (config.isVar("colorR")) sprite.color.r = config.getFloatValue("colorR");
		if (config.isVar("colorG")) sprite.color.g = config.getFloatValue("colorG");
		if (config.isVar("colorB")) sprite.color.b = config.getFloatValue("colorB");

		if (config.isVar("scaleX")) sprite.scale.x = config.getFloatValue("scaleX");
		if (config.isVar("scaleY")) sprite.scale.y = config.getFloatValue("scaleY");

		if (config.isVar("originX")) sprite.origin.x = config.getFloatValue("originX");
		if (config.isVar("originY")) sprite.origin.y = config.getFloatValue("originY");

		if (config.isVar("shader")) sprite.shader = shaders()[config.getStringValue("shader")];

		return sprite;
	}

	void Sprite::useShader() {
		if (!shader) {
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

		glBindTexture(GL_TEXTURE_2D, texture.texture);

		transform = glm::mat4(1);
		transform = glm::translate(transform, glm::vec3(position - camera().position - origin, 0));
		transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));
		transform = glm::scale(transform, glm::vec3(scale, 1.0f));

		shader->use();
		shader->setMat("Proj", GL_Context::proj);
		shader->setMat("Transform", transform);
		shader->setVec4f("Color", color);
	}

	void Sprite::draw() {
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}