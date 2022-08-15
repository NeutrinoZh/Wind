#include "Window.h"

namespace WindEngine {
	int       Window::fps = 0;
	float     Window::delta = 0;
	bool      Window::activeLoop = true;
	Uint32    Window::minFrameTime;
	glm::vec2 Window::size;

	void(*Window::Start)(void) = nullptr;
	void(*Window::Update)(void) = nullptr;
	bool(*Window::Exit)(void) = nullptr;
	
	SDL_Window* Window::window = nullptr;
	SDL_Event Window::event;

	namespace {
		Uint32 beginFrameTime, endFrameTime,
			countFrame = 0, lastTime = 0;
	}

	bool Window::init(JText::Object& obj_config) {
		Log::info() << "Window Init";

		struct _ {
			std::string title = "WindEngine";
			int sizeW = 800, sizeH = 600;
			float minFrameTime = 0.15f;
			Uint32 flags = SDL_WINDOW_SHOWN;

			_(JText::Object& config) {
				title = config["title"]._str(title);
				
				sizeW = config["size"][0]._int(sizeW);
				sizeH = config["size"][1]._int(sizeH);

				minFrameTime = config["minFrameTime"]._float(minFrameTime);

				for (auto obj_flag : config["flags"].children) {
					std::string flag = obj_flag.second->_str("");
					if		(flag == "SDL_WINDOW_OPENGL")			   flags |= SDL_WINDOW_OPENGL;
					else if (flag == "SDL_WINDOW_FULLSCREEN")		   flags |= SDL_WINDOW_FULLSCREEN;
					else if (flag == "SDL_WINDOW_FULLSCREEN_DESKTOP")  flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
					else if (flag == "SDL_WINDOW_RESIZABLE")		   flags |= SDL_WINDOW_RESIZABLE;
					else if (flag == "SDL_WINDOW_BORDERLESS")		   flags |= SDL_WINDOW_BORDERLESS;
					else if (flag == "SDL_WINDOW_HIDDEN")			   flags |= SDL_WINDOW_HIDDEN;
				}
			}
		} config(obj_config);

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

		Window::minFrameTime = config.minFrameTime;

		int w, h;
		SDL_GetWindowSize(Window::window, &w, &h);
		Window::size = { static_cast<float>(w), static_cast<float>(h) };

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
			delta = (endFrameTime - beginFrameTime) / 14.f; // what is 14?
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
		Log::info() << "Free memory from Window";

		HandlerGameController::free();
		SDL_DestroyWindow(window);
	}
}