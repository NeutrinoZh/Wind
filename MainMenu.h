#pragma once
#include "GameServer.h"

namespace Game {
	class MainMenu : public EngineCore::GUIObject {
	private:
		EngineCore::Image* background = NULL;

		EngineCore::Button* playButton = NULL;
		EngineCore::Button* joinButton = NULL;
		EngineCore::Button* settingButton = NULL;
		EngineCore::Button* exitButton = NULL;

		void setButton(EngineCore::Button** button, glm::vec2 position, std::string text) {
			(*button) = new EngineCore::Button();
			(*button)->position = position;
			(*button)->size = {2, 0.6f};
			(*button)->font = EngineCore::fonts()["PixelFont"];
			(*button)->text = text;
			container->add(*button);
		}

		void play() {
			EngineCore::Core::setScene("game");
			EngineCore::Client::connect();
		}
	protected:
		void start() {
			container = new EngineCore::Container();

			background = new EngineCore::Image(EngineCore::textures()["main-menu"]);
			background->size = { 12.8f, 7 };
			background->position.y = 7.4f;

			container->add(background);

			setButton(&playButton,    { 1, 5.0f }, "Play");
			setButton(&joinButton,    { 1, 4.3f }, "Join");
			setButton(&settingButton, { 1, 3.6f }, "Settings");
			setButton(&exitButton,    { 1, 2.9f }, "Exit");

			container->Start();
		}

		void update() {
			container->Update();

			if (playButton->state == EngineCore::Widget::State::CLICK) {
				WinExec((game().path + " server").c_str(), SW_HIDE);
				SDL_Delay(300);
				play();
			}

			if (joinButton->state == EngineCore::Widget::State::CLICK)
				play();

			if (exitButton->state == EngineCore::Widget::State::CLICK)
				EngineCore::Window::quit();
		}

		void draw() {
			container->Draw();
		}
	};
}