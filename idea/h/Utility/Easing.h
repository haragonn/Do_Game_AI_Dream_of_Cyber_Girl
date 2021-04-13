#ifndef INCLUDE_IDEA_EASING_H
#define INCLUDE_IDEA_EASING_H

#include "ideaMath.h"

namespace idea{
	// ease-outï‚ä‘
	inline float EaseOutSine(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (sinf((t * ideaMath::PI) / 2.0f));
	}

	inline float EaseOutQuad(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (t * (2.0f - t));
	}

	inline float EaseOutCubic(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (1.0f - (float)pow(1.0f - t, 3));
	}

	inline float EaseOutQuart(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (1.0f - (float)pow(1.0f - t, 4));
	}

	inline float EaseOutQuint(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (1.0f - (float)pow(1.0f - t, 5));
	}

	inline float EaseOutExpo(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return ((t == 1.0f) ? 1.0f : 1.0f - (float)pow(2, -10.0f * t));
	}

	inline float EaseOutCirc(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (sqrt(1.0f - (float)pow(t - 1.0f, 2)));
	}

	inline float EaseOutBack(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		const float c1 = 1.70158f;
		const float c3 = c1 + 1.0f;

		return (1.0f + c3 * (float)pow(t - 1.0f, 3) + c1 * (float)pow(t - 1.0f, 2));
	}

	inline float EaseOutElastic(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		const float c4 = (2.0f * ideaMath::PI) / 3.0f;

		return (t == 0.0f) ? 0.0f : (t == 1.0f) ? 1.0f : (float)pow(2, -10.0f * t) * sin((t * 10.0f - 0.75f) * c4) + 1.0f;
	}

	inline float EaseOutBounce(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		if(t < 1.0f / d1) {
			return n1 * t * t;
		} else if(t < 2.0f / d1) {
			return n1 * (t -= 1.5f / d1) * t + 0.75f;
		} else if(t < 2.5f / d1) {
			return n1 * (t -= 2.25f / d1) * t + 0.9375f;
		} else {
			return n1 * (t -= 2.625f / d1) * t + 0.984375f;
		}
	}

	// ease-inï‚ä‘
	inline float EaseInSine(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (1.0f - cosf((t * ideaMath::PI) / 2.0f));
	}

	inline float EaseInQuad(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (t * t);
	}

	inline float EaseInCubic(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (t * t * t);
	}

	inline float EaseInQuart(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (t * t * t * t);
	}

	inline float EaseInQuint(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (t * t * t * t * t);
	}

	inline float EaseInExpo(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return ((t == 0.0f) ? 0.0f : (float)pow(2.0f, 10.0f * t - 10.0f));
	}

	inline float EaseInCirc(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (1.0f - sqrt(1.0f - (float)pow(t, 2)));
	}

	inline float EaseInBack(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		const float c1 = 1.70158f;
		const float c3 = c1 + 1.0f;

		return (c3 * t * t * t - c1 * t * t);
	}

	inline float EaseInElastic(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		const float c4 = (2.0f * ideaMath::PI) / 3.0f;

		return ((t == 0.0f) ? 0.0f : t == 1.0f ? 1.0f : -(float)pow(2.0f, 10.0f * t - 10.0f) * sin((t * 10.0f - 10.75f) * c4));
	}

	inline float EaseInBounce(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (1.0f - EaseOutBounce(1.0f - t));
	}

	// ease-in-outï‚ä‘
	inline float EaseInOutSine(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (-(cosf(ideaMath::PI * t) - 1.0f) / 2.0f);
	}

	inline float EaseInOutQuad(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (t * t * (3.0f - 2.0f * t));
	}

	inline float EaseInOutCubic(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return ((t < 0.5f) ? 4.0f * t * t * t : 1.0f - (float)pow(-2.0f * t + 2.0f, 3) / 2.0f);
	}

	inline float EaseInOutQuart(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return ((t < 0.5f) ? 8.0f * t * t * t * t : 1.0f - (float)pow(-2.0f * t + 2.0f, 4) / 2.0f);
	}

	inline float EaseInOutQuint(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return ((t < 0.5f) ? 16.0f * t * t * t * t * t : 1.0f - (float)pow(-2.0f * t + 2.0f, 5) / 2.0f);
	}

	inline float EaseInOutExpo(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return ((t == 0.0f) ? 0.0f : (t == 1.0f) ? 1.0f : (t < 0.5f) ? (float)pow(2, 20.0f * t - 10.0f) / 2.0f
			: (2.0f - (float)pow(2, -20.0f * t + 10.0f)) / 2.0f);
	}

	inline float EaseInOutCirc(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return ((t < 0.5f) ? (1.0f - sqrt(1.0f - (float)pow(2.0f * t, 2))) / 2.0f : (sqrt(1.0f - (float)pow(-2.0f * t + 2.0f, 2)) + 1.0f) / 2.0f);
	}

	inline float EaseInOutBack(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		const float c1 = 1.70158f;
		const float c2 = c1 * 1.525f;

		return ((t < 0.5f) ? ((float)pow(2.0f * t, 2) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f : ((float)pow(2.0f * t - 2.0f, 2) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f);
	}

	inline float EaseInOutElastic(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		const float c5 = (2.0f * ideaMath::PI) / 4.5f;

		return ((t == 0.0f) ? 0.0f : (t == 1.0f) ? 1.0f : (t < 0.5f) ? -((float)pow(2, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f : ((float)pow(2, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f);
	}

	inline float EaseInOutBounce(float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return ((t < 0.5f) ? (1.0f - EaseOutBounce(1.0f - 2.0f * t)) / 2.0f : (1.0f + EaseOutBounce(2.0f * t - 1.0f)) / 2.0f);
	}
}

#endif INCLUDE_IDEA_EASING_H	// #ifndef INCLUDE_IDEA_EASING_H