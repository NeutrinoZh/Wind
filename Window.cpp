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

		Log::info() << "Load window config";

		struct _config {
			std::string title = "Engine";
			int sizeW = 800, sizeH = 600;
			float minDelta = 0.15f;
			Uint32 flags = SDL_WINDOW_SHOWN;

			_config() {
				Config config = ConfigReader::read("./config.txt");

				if (config.isVar("title")) title = config.getStringValue("title");
				if (config.isVar("sizeW")) sizeW = config.getIntValue("sizeW");
				if (config.isVar("sizeH")) sizeH = config.getIntValue("sizeH");
				if (config.isVar("delta")) minDelta = config.getFloatValue("delta");

				if (config.isVar("SDL_WINDOW_OPENGL")) flags |= SDL_WINDOW_OPENGL;
				if (config.isVar("SDL_WINDOW_FULLSCREEN")) flags |= SDL_WINDOW_FULLSCREEN;
				if (config.isVar("SDL_WINDOW_FULLSCREEN_DESKTOP")) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
				if (config.isVar("SDL_WINDOW_RESIZABLE")) flags |= SDL_WINDOW_RESIZABLE;
				if (config.isVar("SDL_WINDOW_BORDERLESS")) flags |= SDL_WINDOW_BORDERLESS;
			}
		} config;

		Log::info() << "SDL init (EVERYTHING)";

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			Log::error() << SDL_GetError();
			return false;
		}
		
		Log::info() << "SDL image init (PNG)";

		if (IMG_Init(IMG_INIT_PNG) == 0) {
			Log::error() << IMG_GetError();
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
			config.title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			config.sizeW, config.sizeH,
			config.flags
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

		Window::minFrameTime = config.minDelta;
		
		int w, h;
		SDL_GetWindowSize(Window::window, &w, &h);
		Window::size = { static_cast<float>(w), static_cast<float>(h) };

		Log::end() << "Window creation procedure finished";
		return true;
	}

	void Window::loop() {
		if (Start)
			Start();

		Log::begin() << "Program loop launched";

		while (activeLoop) {
			beginFrameTime = SDL_GetTicks();

			Keyboard::swapbuffer();
			Mouse::resetWhell();
			HandlerGameController::clear();

			while (SDL_PollEvent(&Window::event)) {
				if (event.type == SDL_QUIT) quit();
				else if (event.type == SDL_KEYDOWN) Keyboard::down(&event);
				else if (event.type == SDL_KEYUP) Keyboard::up(&event);
				else if (event.type == SDL_MOUSEBUTTONDOWN) Mouse::down(&event);
				else if (event.type == SDL_MOUSEBUTTONUP) Mouse::up(&event);
				else if (event.type == SDL_MOUSEWHEEL) Mouse::whell(&event);
				else if (event.type == SDL_MOUSEMOTION) Mouse::move(&event, Window::size);
				else if (event.type == SDL_TEXTINPUT) Input::input(&event);
				else if (event.type == SDL_JOYBUTTONDOWN) HandlerGameController::down(&event);
				else if (event.type == SDL_JOYBUTTONUP) HandlerGameController::up(&event);
				else if (event.type == SDL_JOYDEVICEADDED) HandlerGameController::added(event.jdevice.which);
				else if (event.type == SDL_JOYDEVICEREMOVED) HandlerGameController::removed(event.jdevice.which);
				else if (event.type == SDL_WINDOWEVENT) {
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						int w, h;
						SDL_GetWindowSize(Window::window, &w, &h);
						Window::size = { static_cast<float>(w), static_cast<float>(h) };
					}
				}
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
		Log::info() << "Free memory from SDL";

		HandlerGameController::free();
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
}