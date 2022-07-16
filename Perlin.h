#pragma once
#include "Textures.h"

namespace EngineCore {
	void perlinNoiseSeed(unsigned int _seed);
	float perlinNoise(float fx, float fy);
	float multiPerlinNoise(float fx, float fy, int octaves, float persistence);
}