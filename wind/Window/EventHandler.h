#pragma once
#include "../Net/Net.h"

namespace WindEngine {
	class Keyboard {
	private:
		static std::vector<SDL_Keycode> keys;

		static std::vector<SDL_Keycode> buffer;
		static std::vector<SDL_Keycode> just;

		static void swapbuffer() {
			for (SDL_Keycode key : buffer)
				just.push_back(key);
			buffer.clear();
		};
		static void down(SDL_Event* event);
		static void up(SDL_Event* event);

		friend class Window;
	public:
		static bool isJustPressed(SDL_Keycode key);
		static bool isPressed(SDL_Keycode key) {
			return (std::find(keys.begin(), keys.end(), key) != keys.end());
		};
	};

	class Mouse {
	private:
		static std::vector<Uint32> buttons;
		static glm::vec2 position;
		static glm::vec2 global_position;
		static int y_whell;

		static void down(SDL_Event* event);
		static void up(SDL_Event* event);
		static void whell(SDL_Event* event);
		static void move(SDL_Event* event, glm::vec2 windowSize);
		static void resetWhell() { y_whell = 0; }

		friend class Window;
	public:
		static bool isPressed(Uint32 button) {
			return find(buttons.begin(), buttons.end(), button) != buttons.end();
		}
		static glm::vec2 getPosition() { return position; };
		static glm::vec2 getGlobalPosition() { return global_position; };
		static int getWhell() { return y_whell; };
	};

	class Input {
	private:
		static std::string text;

		static void input(SDL_Event* event) {
			text += event->text.text;
		}

		friend class Window;
	public:
		static void start() {
			SDL_StartTextInput();
		}
		static void stop() {
			SDL_StopTextInput();
		}

		static void set(std::string text) {
			Input::text = text;
		}
		static std::string get() {
			return text;
		}
	};

	class GameController {
	private:
		SDL_GameController* controller;
		std::vector<Uint8> keys;
		Uint32 id;

		void down(Uint8 button) {
			if (std::find(keys.begin(), keys.end(), button) == keys.end())
				keys.push_back(button);
		}
		void up(Uint8 button) {
			if (std::find(keys.begin(), keys.end(), button) != keys.end())
				keys.erase(std::find(keys.begin(), keys.end(), button));
		}

		friend class HandlerGameController;
	public:
		GameController(SDL_GameController* controller, Uint32 id) :
			controller(controller), id(id) {};
		~GameController() {
			SDL_GameControllerClose(controller);
		}

		Sint16 getAxis(Uint32 axis) {
			return SDL_GameControllerGetAxis(controller, SDL_GameControllerAxis(axis));
		}
		bool getJustButton(SDL_GameControllerButton button) {
			return std::find(keys.begin(), keys.end(), (Uint8)button) != keys.end();
		}
		bool getButton(SDL_GameControllerButton button) {
			return SDL_GameControllerGetButton(controller, button);
		}

		Uint32 getID() {
			return id;
		}
		SDL_GameController* get() {
			return controller;
		}
	};

	class HandlerGameController {
	private:
		static std::vector<GameController*> controllers;

		static void added(Uint32 controllerID);
		static void removed(Uint32 controllerID);
		static void free() {
			for (size_t i = 0; i < controllers.size(); ++i)
				removed(controllers[i]->id);
		}
		static void down(SDL_Event* event) {
			get(event->jbutton.which)->down(event->jbutton.button);
		}
		static void up(SDL_Event* event) {
			get(event->jbutton.which)->up(event->jbutton.button);
		}
		static void clear() {
			for (GameController* controller : controllers)
				controller->keys.clear();
		}

		static GameController* get(size_t i) {
			for (GameController* controll : controllers)
				if (controll) if (controll->id == i) return controll;
			return nullptr;
		}

		friend class Window;
	public:
		static GameController* getController(size_t i) {
			if (i >= controllers.size()) return nullptr;
			return controllers[i];
		}
	};
}