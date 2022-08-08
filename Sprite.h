#pragma once
#include "Node2D.h"

namespace EngineCore {
	class Sprite : public Behavior {
	private:
		glm::mat4 transform = glm::mat4(1);
	public:
		Shader* shader = nullptr;

		bool flipX = false,
			flipY = false;

		Texture texture;

		glm::vec4 color = { 1, 1, 1, 1 };

		void start() {};
		void update() {};
		void free() {};

		void render();

		static Sprite* builder(std::string path);
	};
}