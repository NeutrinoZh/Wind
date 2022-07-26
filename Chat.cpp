#include "Chat.h"

namespace EngineCore {
	void Chat::update() {
		for (Uint32 i = 0; i < chat.size(); ++i)
			chat[i]->Update();

		if (chat.empty())
			lastDestroy = clock();
		if (clock() > lastDestroy + 5000) {
			chat[0]->Free();
			delete chat[0];
			chat.erase(chat.begin());
			lastDestroy = clock();
		}
	}

	void Chat::draw() {
		for (Uint32 i = 0; i < chat.size(); ++i)
			chat[i]->Draw();
	}

	void Chat::free() {
		for (Uint32 i = 0; i < chat.size(); ++i)
			if (chat[i]) {
				chat[i]->Free();
				delete chat[i];
			}
	}

	void Chat::send(std::string text) {
		if (!font) return;

		for (EngineCore::Label* label : chat)
			label->position.y += label->size.y;

		chat.push_back(new EngineCore::Label());
		chat.back()->font = font;
		chat.back()->Start();
		chat.back()->setText(text);
		chat.back()->position = position;
	}
}