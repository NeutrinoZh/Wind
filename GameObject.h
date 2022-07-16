#pragma once
#include "TileMap.h"

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

		Sprite sprite; // !

		void Start();
		void NetUpdate();
		void Update();
		void Draw();
		void Free();

		std::vector<GameObject*> getObjects();
		void DeleteObject(GameObject* object);
		void AddObject(GameObject* object);

		static void builder(std::string path, GameObject* object);
	};
}