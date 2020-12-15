//
// Created by Tobias on 12/14/2020.
//

#ifndef NPP_TABLET_UTIL_HPP
#define NPP_TABLET_UTIL_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// FIXME somehow don't rely on this include
#include "Tablet.hpp"

namespace npp::util {

enum class PointerType {
	POINTER,
	TOUCH,
	PEN,
	MOUSE,
	TOUCHPAD,
	INVALID
};

constexpr PointerType win32_ptype_to_lib(POINTER_INPUT_TYPE type) {
	switch (type) {
	case PT_POINTER:
		return PointerType::POINTER;
	case PT_TOUCH:
		return PointerType::TOUCH;
	case PT_PEN:
		return PointerType::PEN;
	case PT_MOUSE:
		return PointerType::MOUSE;
	case PT_TOUCHPAD:
		return PointerType::TOUCHPAD;
	default:
		return PointerType::INVALID;
	}
}

constexpr const char *ptype_name(PointerType type) {
	switch (type) {
	case PointerType::POINTER:
		return "POINTER";
	case PointerType::TOUCH:
		return "TOUCH";
	case PointerType::PEN:
		return "PEN";
	case PointerType::MOUSE:
		return "MOUSE";
	case PointerType::TOUCHPAD:
		return "TOUCHPAD";
	case PointerType::INVALID:
	default:
		return "INVALID";
	}
}

constexpr const char *ptype_name(POINTER_INPUT_TYPE type) {
	return ptype_name(win32_ptype_to_lib(type));
}

PointerType id_to_type(Tablet::pointerid_t id);

}

#endif // NPP_TABLET_UTIL_HPP
