#pragma once
#include "../../Resource/Resources.h"

namespace WindEngine {
	class FPSCounter {
	private:
		static Uint32
			lastInvoke,
			frameCount,
			lastSecond,
			delta,
			minFrameTime,
			fps;
		static const Uint32 deltaDivider = 14;
	public:
		static Uint32 _delta();
		static Uint32 count(Uint32 max=0);
	};

	#define delta(x) ((x) * wd::FPSCounter::_delta())
}