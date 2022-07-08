#include "EventHandler.h"

namespace EngineCore {
	//========================Keyboard====================================//

	std::vector<SDL_Keycode> Keyboard::keys;
	std::vector<SDL_Keycode> Keyboard::buffer;
	std::vector<SDL_Keycode> Keyboard::just;

	void Keyboard::down(SDL_Event* event) {
		if (!isPressed(event->key.keysym.sym)) {
			keys.push_back(event->key.keysym.sym);
			if (std::find(just.begin(), just.end(), event->key.keysym.sym) == just.end())
				buffer.push_back(event->key.keysym.sym);
		}
	}
	
	void Keyboard::up(SDL_Event* event) {
		keys.erase(std::find(keys.begin(), keys.end(), event->key.keysym.sym));

		if (std::find(just.begin(), just.end(), event->key.keysym.sym) != just.end())
			just.erase(std::find(just.begin(), just.end(), event->key.keysym.sym));
	}

	bool Keyboard::isJustPressed(SDL_Keycode key) {
		if (isPressed(key))
			return std::find(buffer.begin(), buffer.end(), key) != buffer.end();
		return false;
	}

	//==========================Mouse=====================================//

	std::vector<Uint32> Mouse::buttons;
	glm::vec2 Mouse::global_position;
	glm::vec2 Mouse::position;
	int Mouse::y_whell;

	void Mouse::down(SDL_Event* event) {
		if (!isPressed(event->button.button))
			buttons.push_back(event->button.button);
	}
	void Mouse::up(SDL_Event* event) {
		if (isPressed(event->button.button))
			buttons.erase(std::find(buttons.begin(), buttons.end(), event->button.button));
	}
	void Mouse::move(SDL_Event* event, glm::vec2 windowSize) {
		global_position.x = static_cast<float>(event->button.x);
		global_position.y = static_cast<float>(event->button.y);
		position.x = global_position.x / windowSize.x;
		position.y = global_position.y / windowSize.y;
	}
	void Mouse::whell(SDL_Event* event) {
		y_whell = event->wheel.y;
	}

	//=======================Input========================================//

	std::string Input::text;

	//=======================GameController===============================//

	std::vector<GameController*> HandlerGameController::controllers;

	void HandlerGameController::added(Uint32 _controllerID) {
		Uint32 controllerID = SDL_JoystickGetDeviceInstanceID(_controllerID);
		Log::info() << "Started init controller (" << controllerID << ")";
		if (SDL_IsGameController(_controllerID)) {
			SDL_GameController* controller = SDL_GameControllerOpen(_controllerID);
			if (controller) {
				Log::info() << "Successfull finish init controller:" << SDL_GameControllerName(controller);
				controllers.push_back(new GameController(controller, controllerID));
			} else Log::error() << "Couldn't open controller (" << controllerID << ")";
		} else Log::error() << "Input device not supported (" << controllerID << ")";
	}

	void HandlerGameController::removed(Uint32 controllerID) {
		Log::info() << "Started remove controller (" << controllerID << ")";

		for (int i = 0; i < controllers.size(); ++i)
			if (controllers[i]->id == controllerID) {
				delete controllers[i];
				controllers.erase(controllers.begin() + i);

				Log::info() << "Successfull finish remove controller (" << controllerID << ")";
				return;
			}

		Log::error() << "Controller does not exist (" << controllerID << "), failed to delete\n";
	}
}