//
// Created by Tobias on 10/7/2020.
//

#ifndef CPP_UTIL_H
#define CPP_UTIL_H

#include <concepts>
#include <ctime>
#include <limits>
#include <random>

namespace npp::rnd {

std::mt19937 &twister();

template<typename T>
concept Integer = std::integral<T> && !std::same_as<T, bool> &&
				  !std::same_as<T, char> && !std::same_as<T, unsigned char>;

template<typename T>
concept FloatingPoint = std::floating_point<T>;

template<typename T>
concept Boolean = std::same_as<T, bool>;

template<Integer I>
I random(I min, I max) {
	return std::uniform_int_distribution<I>{min, max}(twister());
}

template<FloatingPoint F>
F random(F min, F max) {
	return std::uniform_real_distribution<F>{min, max}(twister());
}

template<typename T>
	requires Integer<T> || FloatingPoint<T> struct Range {
	T min;
	T max;

	T random() const { return rnd::random<T>(min, max); }
};

template<typename T>
T random(Range<T> range) {
	return range.random();
}

template<Integer I>
I random() {
	return random<I>(std::numeric_limits<I>::min(),
					 std::numeric_limits<I>::max());
}

template<typename T>
T random_01() {
	return random<T>(0, 1);
}

template<Boolean B>
B random() {
	return random_01<unsigned int>() == 0;
}

template<FloatingPoint F>
F random() {
	// lowest would enable floating point numbers to be negative
	// this crashes uniform_real_distribution because max - min <= max has to be
	// true that's why we randomly flip the sign here
	return random<F>(std::numeric_limits<F>::min(),
					 std::numeric_limits<F>::max()) *
		   (random_01<unsigned short>() == 0 ? -1 : 1);
}

} // namespace rnd

#endif // CPP_UTIL_H
