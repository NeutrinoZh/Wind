#pragma once
#include "GameObject.h"

namespace EngineCore {

	void (*Start) (void);
	void (*Update) (void);

	GameObject* scene = new GameObject();

	namespace {
		bool preInit() {
			return EngineCore::GL_Context::preInit();
		}

		bool postInit() {
			return (
				EngineCore::GL_Context::postInit() &&
				EngineCore::Net::init()
			);
		}

		void start() {
			Log::begin() << "Start load resource";

			EngineCore::textures().loadFolder("asset/meta-textures/");
			EngineCore::shaders().loadFolder("asset/meta-shaders/");
			EngineCore::animations().loadFolder("asset/animations/");

			Log::end() << "Success finish load resource";

			Start();
			scene->Start();
		}

		void draw() {
			scene->Draw();
		}

		void update() {
			Net::update();

			Update();
			scene->Update();

			EngineCore::GL_Context::draw();
		}

		void free() {
			Log::begin() << "Memory cleaning procedure started";

			EngineCore::shaders().freeAll();
			EngineCore::textures().freeAll();
			
			EngineCore::Net::free();
			EngineCore::Window::free();
			EngineCore::GL_Context::free();
			
			scene->Free();
			delete scene;

			Log::end() << "Memory cleaning procedure finished";
		}
	}

	int loop() {
		Log::config([](auto& self) {
			self.outConsole = true;
			self.folderToSave = "./asset/logs/";
			self.numSpace = 2;
		});

		EngineCore::Window::PreInit = preInit;
		EngineCore::Window::PostInit = postInit;
		EngineCore::Window::Start = start;
		EngineCore::Window::Update = update;
		EngineCore::GL_Context::Draw = draw;

		if (!EngineCore::Window::init()) {
			Log::error() << "Error init window";
			return EXIT_FAILURE;
		}

		EngineCore::Window::loop();
		free();

		return EXIT_SUCCESS;
	}
}