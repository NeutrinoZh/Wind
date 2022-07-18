#include "Tile.h"

namespace EngineCore {
	Tile Tile::builder(std::string path) {
		Tile tile;

		Config config = ConfigReader::read(path);

		if (config.isVar("texture")) tile.texture = textures()[config.getStringValue("texture")];
		if (config.isVar("solid"))	 tile.solid = config.getBoolValue("solid");

		if (config.isVar("colorR")) tile.color.r = config.getFloatValue("colorR");
		if (config.isVar("colorG")) tile.color.g = config.getFloatValue("colorG");
		if (config.isVar("colorB")) tile.color.b = config.getFloatValue("colorB");

		return tile;
	}
}