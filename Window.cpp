#include "Window.h"

namespace EngineCore {
	int       Window::fps = 0;
	float     Window::delta = 0;
	bool      Window::activeLoop = true;
	Uint32    Window::minFrameTime;
	glm::vec2 Window::size;

	void(*Window::Start)(void) = nullptr;
	void(*Window::Update)(void) = nullptr;
	bool(*Window::Exit)(void) = nullptr;
	bool(*Window::PreInit)(void) = nullptr;
	bool(*Window::PostInit)(void) = nullptr;
	
	SDL_Window* Window::window = nullptr;
	SDL_Event Window::event;

	namespace {
		Uint32 beginFrameTime, endFrameTime,
			countFrame = 0, lastTime = 0;
	}

	bool Window::init() {
		Log::begin() << "Window creation procedure started";

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			Log::error() << SDL_GetError();
			return false;
		}
		
		Log::info() << "Calling the preinitialization method";

		if (PreInit)
			if (!PreInit()) {
				Log::error() << "The preinitialization method returned an error.";
				return false;
		}

		Log::info() << "Create a window";

		window = SDL_CreateWindow(
			"Demo",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			800, 600, SDL_WINDOW_SHOWN
		);

		if (!window) {
			Log::error() << SDL_GetError();
			return false;
		}

		Log::info() << "Calling the postinitialization method";

		if (PostInit)
			if (!PostInit()) {
				Log::error() << "The postinitialization method returned an error.";
				return false;
		}

		Log::end() << "Window creation procedure finished";
		return true;
	}

	void Window::loop() {
		Log::begin() << "Program loop launched";

		if (Start)
			Start();

		while (activeLoop) {
			beginFrameTime = SDL_GetTicks();

			while (SDL_PollEvent(&Window::event)) {
				if (event.type == SDL_QUIT) quit();
			}

			if (Update)
				Update();

			countFrame++;
			if (SDL_GetTicks() > lastTime + 1000) {
				fps = countFrame;
				countFrame = 0;
				lastTime = SDL_GetTicks();
			}

			endFrameTime = SDL_GetTicks();
			if (endFrameTime - beginFrameTime < minFrameTime)
				SDL_Delay(minFrameTime - (endFrameTime - beginFrameTime));

			endFrameTime = SDL_GetTicks();
			delta = (endFrameTime - beginFrameTime) / 14.f;
		}

		Log::end() << "Program cycle break";
	}

	void Window::quit() {
		if (Exit) {
			Window::activeLoop = Exit();
		} else activeLoop = false;

		Log::info() << "Program exit request:"
			<< (activeLoop ? "Unsuccessful" : "Successfull");
	}

	void Window::free() {
		Log::info() << "Memory cleaning procedure started";

		SDL_DestroyWindow(window);
		SDL_Quit();

		Log::info() << "Memory cleaning procedure finished";
	}
}