#pragma once
#include "../../Graphics/Texture/Texture.h"

namespace WindEngine {
	class Nodes : public Resource<JText::Object> {
	public:
		Nodes(std::string name) : Resource<JText::Object>(name) {};
		static Nodes* nodes;

		void load_o(JText::Object& node);
		void free(JText::Object) {};
	};

	Nodes& nodes();
}