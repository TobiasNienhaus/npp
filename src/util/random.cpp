//
// Created by Tobias on 10/7/2020.
//

#include "random.h"

namespace npp::rnd {

std::mt19937 &twister() {
	static std::mt19937 twister{std::random_device{}()};
	return twister;
}

} // namespace rnd