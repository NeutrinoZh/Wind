#pragma once
#include "Physics.h"	

namespace EngineCore {

	struct Core {
		static void (*Start) (void);
		static void (*Update) (void);

		static GameObject* scene;
		static int loop();
	};
	
	namespace {
		clock_t lastTimeCallNetUpdate = 0;

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

			Core::Start();
			Core::scene->Start();
		}

		void draw() {
			Core::scene->Draw();
		}

		void update() {
			Net::update();

			if (Core::Update)
				Core::Update();
			
			Core::scene->Update();

			if (clock() > lastTimeCallNetUpdate + 15 && !Net::isServer) {
				Core::scene->NetUpdate();
				lastTimeCallNetUpdate = clock();
			}

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