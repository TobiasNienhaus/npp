//
// Created by Tobias on 1/18/2021.
//
#include "colorspaces.hpp"
#include <algorithm>
#include <cmath>

namespace npp::graphics {

float rgb_to_hue(float r, float g, float b) {
	float min = std::min(r, std::min(g, b));
	float max = std::max(r, std::max(g, b));

	float hue;
	if (r == max) {
		hue = (g - b) / (max - min);
	} else if (g == max) {
		hue = 2.0f + (b - r) / (max - min);
	} else {
		hue = 4.0f + (r - g) / (max - min);
	}
	hue = std::fmod(hue, 6.0f);
	if (hue < 0.f) { hue += 6.0f; }
	return hue / 6.0f;
}

RgbColor hue_to_rgb(float hue) {
	hue *= 360.f;
	float hh, p, q, t, ff;
	long i;
	RgbColor out;

	float s = 1.0f;
	float v = 1.0f;

	if(s <= 0.0) {       // < is bogus, just shuts up warnings
		out.r = v;
		out.g = v;
		out.b = v;
		return out;
	}
	hh = hue;
	if(hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = v * (1.0 - s);
	q = v * (1.0 - (s * ff));
	t = v * (1.0 - (s * (1.0 - ff)));

	switch(i) {
	case 0:
		out.r = v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = v;
		break;
	case 5:
	default:
		out.r = v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}

}