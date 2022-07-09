#pragma once
#include "Resource.h"

namespace EngineCore {
	
	class Textures : public Resource<UINT32> {
	public:
		Textures(std::string name) : Resource<UINT32>(name) {};
		static Textures* textures;

		UINT32 load(std::string);
		void free(UINT32);
	};
	
	Textures& textures();
}