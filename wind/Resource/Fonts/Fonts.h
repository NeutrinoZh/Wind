#pragma once
#include "../Nodes/Nodes.h"

namespace WindEngine {
	class Fonts : public Resource<TTF_Font*> {
	public:
		Fonts(std::string name) : Resource<TTF_Font*>(name) {};
		static Fonts* fonts;

		TTF_Font* load(std::string);
		void free(TTF_Font*);
	};

	Fonts& fonts();
}