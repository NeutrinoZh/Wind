#pragma once
#include "../Nodes/Nodes.h"

namespace WindEngine {
	class Fonts : public Resource<TTF_Font*> {
	public:
		Fonts(std::string name) : Resource<TTF_Font*>(name) {};
		static Fonts* fonts;

		void load_o(JText::Object& font);
		void free(TTF_Font*);
	};

	Fonts& fonts();
}