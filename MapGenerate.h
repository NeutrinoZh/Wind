#pragma once
#include "Player.h"

namespace Game {
	void mapGenerate(std::string path) {
		struct _config {
			float tempGrid = 32.f;
			Uint32 tempOctaves = 3;
			float tempPersistence = 0.8f;

			std::string distanceMethod = "Pythagoras";

			float highGrid = 48.f;
			Uint32 highOctaves = 2;
			float highPersistence = 0.8f;

			float distanceA = 0.05;
			float distanceB = 1;
			float distanceC = 2;

			_config(std::string path) {
				EngineCore::Config config = EngineCore::ConfigReader::read(path);

				if (config.isVar("tempGrid"))		 tempGrid = config.getFloatValue("tempGrid");
				if (config.isVar("tempOctaves"))	 tempOctaves = config.getIntValue("tempOctaves");
				if (config.isVar("tempPersistence")) tempPersistence = config.getFloatValue("tempPersistence");

				if (config.isVar("distanceMethod")) distanceMethod = config.getStringValue("distanceMethod");

				if (config.isVar("highGrid"))		 highGrid = config.getFloatValue("highGrid");
				if (config.isVar("highOctaves"))	 highOctaves = config.getIntValue("highOctaves");
				if (config.isVar("highPersistence")) highPersistence = config.getFloatValue("highPersistence");

				if (config.isVar("distanceA")) distanceA = config.getFloatValue("distanceA");
				if (config.isVar("distanceB")) distanceB = config.getFloatValue("distanceB");
				if (config.isVar("distanceC")) distanceC = config.getFloatValue("distanceC");
			}
		} config(path);

		Uint32 w = game().background->tilemap.map.size(),
			   h = game().background->tilemap.map[0].size();

		for (Uint32 x = 0; x < w; ++x)
			for (Uint32 y = 0; y < h; ++y)
				game().background->tilemap.map[x][y] = 5;

		//=============================================//
		EngineCore::perlinNoiseSeed(game().seed);

		std::vector<std::vector<float>> temp;
		temp.resize(w);
		for (Uint32 x = 0; x < w; ++x) {
			temp[x].resize(h);
			for (Uint32 y = 0; y < h; ++y)
				temp[x][y] = EngineCore::multiPerlinNoise(x / config.tempGrid, y / config.tempGrid,
					config.tempOctaves, config.tempPersistence);
		}
		//=============================================//

		// 1 - dirt
		// 2 - grass
		// 3 - sand
		// 4 - stone
		// 5 - water

		EngineCore::perlinNoiseSeed(game().seed - rand());
		for (Uint32 x = 0; x < w; ++x) {
			for (Uint32 y = 0; y < h; ++y) {
				float high, dist;

				if (config.distanceMethod == "Pythagoras")
					dist = std::sqrt(std::pow((w / 2.f) - x, 2) + std::pow((h / 2.f) - y, 2)) / (w / 1.5f);
				else
					dist = (std::abs((w / 2.f) - x) + std::abs((h / 2.f) - y)) / w;

				high = EngineCore::multiPerlinNoise(x / config.highGrid, y / config.highGrid,
					config.highOctaves, config.highPersistence);

				// h = h + a - b * d ^ c
				high += config.distanceA - config.distanceB * pow(dist, config.distanceC);

				if (high > 0.09) {
					if (temp[x][y] > 0.05) game().background->tilemap.map[x][y] = 1;
					else				   game().background->tilemap.map[x][y] = 2;

					if (high > 0.2) game().background->tilemap.map[x][y] = 4;
					else if (rand() % 8 == 0) game().foreground->tilemap.map[x][y] = 1;
				} else if (high > 0.04) game().background->tilemap.map[x][y] = 3;
			}
		}
	}
}