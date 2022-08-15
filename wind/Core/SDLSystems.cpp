#include "SDLSystems.h"

namespace WindEngine {
	Uint32 SDLSystems::SDL_Flags = NULL,
		   SDLSystems::IMG_Flags = NULL;

	bool SDLSystems::TTF = false,
		 SDLSystems::NET = false;

	bool SDLSystems::init(JText::Object& config) {
		for (auto flag : config.children) {
			std::string value = flag.second->_str("");

			if (value == "SDL_INIT_AUDIO")				 SDL_Flags |= SDL_INIT_AUDIO;
			else if (value == "SDL_INIT_EVENTS")		 SDL_Flags |= SDL_INIT_EVENTS;
			else if (value == "SDL_INIT_GAMECONTROLLER") SDL_Flags |= SDL_INIT_GAMECONTROLLER;
			else if (value == "SDL_INIT_HAPTIC")		 SDL_Flags |= SDL_INIT_HAPTIC;
			else if (value == "SDL_INIT_JOYSTICK")		 SDL_Flags |= SDL_INIT_JOYSTICK;
			else if (value == "SDL_INIT_NOPARACHUTE")	 SDL_Flags |= SDL_INIT_NOPARACHUTE;
			else if (value == "SDL_INIT_SENSOR")		 SDL_Flags |= SDL_INIT_SENSOR;
			else if (value == "SDL_INIT_TIMER")			 SDL_Flags |= SDL_INIT_TIMER;
			else if (value == "SDL_INIT_VIDEO")			 SDL_Flags |= SDL_INIT_VIDEO;
			else if (value == "SDL_INIT_EVERYTHING") {
				SDL_Flags = SDL_INIT_EVERYTHING;
				break;
			}
		}

		for (auto flag : config.children) {
			std::string value = flag.second->_str("");

			if (value == "IMG_INIT_JPG")	   IMG_Flags |= IMG_INIT_JPG;
			else if (value == "IMG_INIT_PNG")  IMG_Flags |= IMG_INIT_PNG;
			else if (value == "IMG_INIT_TIF")  IMG_Flags |= IMG_INIT_TIF;
			else if (value == "IMG_INIT_WEBP") IMG_Flags |= IMG_INIT_WEBP;
		}

		for (auto flag : config.children)
			if (flag.second->_str("") == "TTF_INIT") {
				TTF = true;
				break;
			}

		for (auto flag : config.children)
			if (flag.second->_str("") == "NET_INIT") {
				NET = true;
				break;
			}

		if (SDL_Flags) {
			Log::info() << "SDL Init";

			if (SDL_Init(SDL_Flags) != 0) {
				Log::error() << SDL_GetError();
				return false;
			}

			Log::info() << "SDL Version: " << SDL_MAJOR_VERSION << "."
										   << SDL_MINOR_VERSION << "."
										   << SDL_PATCHLEVEL;
		} else {
			Log::error() << "SDL init flags been NULL";
			return false;
		}

		if (IMG_Flags) {
			Log::info() << "SDL Image Init";

			if (IMG_Init(IMG_Flags) == 0) {
				Log::error() << IMG_GetError();
				return false;
			}

			Log::info() << "SDL Image Version: " << SDL_IMAGE_MAJOR_VERSION << "."
												 << SDL_IMAGE_MINOR_VERSION << "."
												 << SDL_IMAGE_PATCHLEVEL;
		}

		if (TTF) {
			Log::info() << "SDL TTF init";

			if (TTF_Init() == -1) {
				Log::error() << TTF_GetError();
				return false;
			}

			Log::info() << "SDL TTF Version: " << SDL_TTF_MAJOR_VERSION << "."
											   << SDL_TTF_MINOR_VERSION << "."
											   << SDL_TTF_PATCHLEVEL;
		}

		if (NET) {
			Log::info() << "SDL Net init";
			if (SDLNet_Init() == -1) {
				Log::error() << SDLNet_GetError();
				return false;
			}

			Log::info() << "SDL Net Version: " << SDL_NET_MAJOR_VERSION << "."
											   << SDL_NET_MINOR_VERSION << "."
											   << SDL_NET_PATCHLEVEL;
		}

		return true;
	}
	void SDLSystems::free() {
		Log::info() << "SDL Net quit";
		SDLNet_Quit();

		Log::info() << "SDL Image quit";
		IMG_Quit();

		Log::info() << "SDL TTF quit";
		TTF_Quit();

		Log::info() << "SDL quit";
		SDL_Quit();
	}
}