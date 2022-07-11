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

		return sprite;
	}

	void Sprite::useShader() {
		if (!shader) {
			Log::error() << "When drawing shader been NULL";
			return;
		}

		glBindTexture(GL_TEXTURE_2D, texture);

		transform = glm::mat4(1);
		transform = glm::translate(transform, glm::vec3(position + origin, 0));
		transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0, 0, 1));
		transform = glm::translate(transform, glm::vec3(-origin, 0));
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