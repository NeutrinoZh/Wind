#pragma once
#include "../../Graphics/Animation/Animation.h"

namespace WindEngine {
	class Animations : public Resource<Animation> {
	public:
		Animations(std::string name) : Resource<Animation>(name) {};
		static Animations* animations;

		Animation load(std::string path);
		void free(Animation animation);
;	};

	Animations& animations();
}