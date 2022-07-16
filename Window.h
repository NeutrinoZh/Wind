#pragma once
#include "EventHandler.h"

namespace EngineCore {
	class Window {
	public:
		static void (*Start) (void);
		static void (*Update) (void);
		static bool (*Exit) (void);
		static bool (*PreInit) (void);
		static bool (*PostInit) (void);

		static bool activeLoop;
		static glm::vec2 size;
		static Uint32 minFrameTime;
		static int fps;
		static float delta;

		static SDL_Window* window;
		static SDL_Event event;

		static bool init();
		static void loop();
		static void free();
		static void quit();
	};

	#define delta(a) (a) * EngineCore::Window::delta
}