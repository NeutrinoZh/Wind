#pragma once
#include "Animations.h"

namespace EngineCore {
	class GameObject {
	protected:
		std::vector<GameObject*> objects;
	protected:
		virtual void netUpdate() {};

		virtual void start() {};
		virtual void update() {};
		virtual void draw() {};
	public:

		Sprite sprite;

		void Start();
		void NetUpdate();
		void Update();
		void Draw();
		void Free();

		virtual void ProcessNetData(byte* data, Uint32 len) {};

		void AddObject(GameObject* object);

		static void builder(std::string path, GameObject* object);
	};
}