#pragma once
#include "../Resource/Textures/Textures.h"

namespace WindEngine {
	void perlinNoiseSeed(unsigned int _seed);
	float perlinNoise(float fx, float fy);
	float multiPerlinNoise(float fx, float fy, int octaves, float persistence);
}