#pragma once
#include "Shaders.h"

namespace EngineCore {
	class Sprite {
	private:
		glm::mat4 transform = glm::mat4(1);
	public:
		Shader* shader = nullptr;

		glm::vec2 position = { 0, 0 },
				  scale = { 1, 1 };
		float angle = 0;
		bool flipX = false,
			 flipY = false;

		Texture texture;	

		glm::vec4 color = { 1, 1, 1, 1 };
		glm::vec2 origin = { 0, 0 };

		void useShader();
		void draw();

		static Sprite builder(std::string path);
	};
}