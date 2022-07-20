#pragma once
#include "GUI.h"	

namespace EngineCore {

	struct Core {
		static void (*Start) (void);
		static void (*Update) (void);
		static void (*Draw) (void);

		static GameObject* scene;
		static int loop();
	};
	
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
			GUI::init();

			Log::begin() << "Start load resource";

			EngineCore::textures().loadFolder("asset/meta-textures/");
			EngineCore::shaders().loadFolder("asset/meta-shaders/");
			EngineCore::fonts().loadFolder("asset/meta-fonts/");
			EngineCore::animations().loadFolder("asset/animations/");

			Log::end() << "Success finish load resource";

			GUI::shader = shaders()["Shader"]; // !

			if (Core::Start)
				Core::Start();
			
			Core::scene->Start();
		}

		void draw() {
			Core::scene->Draw();

			if (Core::Draw)
				Core::Draw();
		}

		void update() {
			Net::update();

			if (Core::Update)
				Core::Update();
			
			Core::scene->Update();

			EngineCore::GL_Context::draw();
		}

		void free() {
			Log::begin() << "Memory cleaning procedure started";

			EngineCore::shaders().freeAll();
			EngineCore::textures().freeAll();
			
			EngineCore::Net::free();
			EngineCore::Window::free();
			EngineCore::GL_Context::free();
			
			Core::scene->Free();
			delete Core::scene;

			Log::end() << "Memory cleaning procedure finished";
		}
	}
}