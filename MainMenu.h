#pragma once
#include "GameServer.h"

namespace Game {
	class MainMenu {
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
			self.container->add(*button);
		}

		static void play() {
			self.container->active = false;

			game().background = new EngineCore::TileMapObject(
				EngineCore::TileMap::builder("./asset/tilemap/background.meta")
			);
			EngineCore::Core::scene->AddObject(game().background);

			game().foreground = new EngineCore::TileMapObject(
				EngineCore::TileMap::builder("./asset/tilemap/foreground.meta")
			);
			EngineCore::Core::scene->AddObject(game().foreground);

			EngineCore::Client::connect();
		}

	public:
		EngineCore::Container* container = NULL;
		static MainMenu self;

		static void Start() {
			self.container = new EngineCore::Container();

			self.background = new EngineCore::Image(EngineCore::textures()["main-menu"]);
			self.background->size = { 12.8f, 7 };
			self.background->position.y = 7.4f;

			self.container->add(self.background);

			self.setButton(&self.playButton,    { 1, 5.0f }, "Play");
			self.setButton(&self.joinButton,    { 1, 4.3f }, "Join");
			self.setButton(&self.settingButton, { 1, 3.6f }, "Settings");
			self.setButton(&self.exitButton,    { 1, 2.9f }, "Exit");

			self.container->Start();
		}

		static void Update() {
			if (!self.container->active) return;

			self.container->Update();

			if (self.playButton->state == EngineCore::Widget::State::CLICK) {
				WinExec((game().path + " server").c_str(), SW_HIDE);
				SDL_Delay(300);
				self.play();
			}

			if (self.joinButton->state == EngineCore::Widget::State::CLICK) {
				self.play();
			}

			if (self.exitButton->state == EngineCore::Widget::State::CLICK)
				EngineCore::Window::quit();
		}

		static void Draw() {
			self.container->Draw();
		}
	};

	MainMenu MainMenu::self = MainMenu();
}