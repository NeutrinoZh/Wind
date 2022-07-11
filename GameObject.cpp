#include "GameObject.h"

namespace EngineCore {
	void GameObject::Start() {
		start();

		for (GameObject* object : objects)
			object->Start();
	}

	void GameObject::Update() {
		update();

		for (GameObject* object : objects)
			object->Update();
	}

	void GameObject::Draw() {
		draw();

		if (sprite.shader) {
			sprite.useShader();
			sprite.draw();
		}

		for (GameObject* object : objects)
			object->Draw();
	}

	void GameObject::Free() {
		for (GameObject* object : objects)
			if (object) {
				object->Free();
				delete object;
			}
	}

	void GameObject::AddObject(GameObject* object) {
		objects.push_back(object);
	}

	void GameObject::builder(std::string path, GameObject* object) {
		Config config = ConfigReader::read(path);

		if (config.isVar("sprite"))
			object->sprite = Sprite::builder(config.getStringValue("sprite"));

		if (config.isVar("shader"))
			object->sprite.shader = shaders()[config.getStringValue("shader")];
	}
}