#pragma once
#include "../../Graphics/Animation/Animation.h"

namespace WindEngine {
	class Animations : public Resource<Animation> { // to *
	public:
		Animations(std::string name) : Resource<Animation>(name) {};
		static Animations* animations;

		void load_o(JText::Object& animation);
		void free(Animation animation);
;	};

	Animations& animations();
}