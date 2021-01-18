//
// Created by Tobias on 1/11/2021.
//

#ifndef NPP_COLORSPACES_HPP
#define NPP_COLORSPACES_HPP

#include <algorithm>
#include <cmath>

namespace npp::graphics {

float rgb_to_hue(float r, float g, float b);

struct RgbColor {
	float r{};
	float g{};
	float b{};
};

RgbColor hue_to_rgb(float hue);

} // namespace npp::graphics

#endif // NPP_COLORSPACES_HPP
