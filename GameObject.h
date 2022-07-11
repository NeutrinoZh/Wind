#pragma once
#include "Sprite.h"

namespace EngineCore {
	class GameObject {
	protected:
		Sprite sprite;
		std::vector<GameObject*> objects;
	protected:
		virtual void start() {};
		virtual void update() {};
		virtual void draw() {};
	public:
		void Start();
		void Update();
		void Draw();
		void Free();

		void AddObject(GameObject* object);

		static void builder(std::string path, GameObject* object);
	};
}