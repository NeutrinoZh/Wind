#include "FPSCounter.h"

namespace WindEngine {
	Uint32 
		FPSCounter::lastInvoke = 0,
		FPSCounter::frameCount = 0,
		FPSCounter::lastSecond = 0,
		FPSCounter::delta = 0,
		FPSCounter::minFrameTime = 0,
		FPSCounter::fps = 0;

	Uint32 FPSCounter::_delta() {
		return delta;
	}

	Uint32 FPSCounter::count(Uint32 max) {
		if (max != 0)
			minFrameTime = 1000 / max;
		else minFrameTime = 0;

		Uint32 newInvoke = SDL_GetTicks();
		if (newInvoke - lastInvoke < minFrameTime)
			SDL_Delay(minFrameTime - (newInvoke - lastInvoke));
		delta = (newInvoke - lastInvoke) / deltaDivider;
		lastInvoke = newInvoke;

		frameCount++;
		if (newInvoke > lastSecond + 1000) {
			lastSecond = newInvoke;
			fps = frameCount;
			frameCount = 0;
		}

		return fps ? fps : frameCount;
	}
}