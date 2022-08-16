#pragma once
#include "../../Graphics/Graphics.h"

namespace WindEngine {
	class Shaders : public Resource<Shader*> {
	public:
		Shaders(std::string name) : Resource<Shader*>(name) {};
		static Shaders* shaders;

		void load_o(JText::Object& shader);
		void free(Shader* shader);
	};

	Shaders& shaders();
}