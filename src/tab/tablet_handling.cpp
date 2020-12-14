//
// Created by Tobias on 12/14/2020.
//

#include "tablet_handling.hpp"

#include <iostream>

#include "tablet_util.hpp"
#include "Tablet.hpp"

namespace npp {

static void on_pointer_down();
static void on_pointer_up();

LRESULT catch_tablet_msgs(WNDPROC next, HWND hwnd, UINT umsg, WPARAM wparam,
						  LPARAM lparam) {
	switch (umsg) {
	case WM_POINTERENTER: {
		auto id = GET_POINTERID_WPARAM(wparam);
		POINTER_INPUT_TYPE type;
		if (!GetPointerType(id, &type)) {
			std::cout << "SOMETHING FAILED!" << GetLastError() << '\n';
		}
		std::cout << util::ptype_name(type) << " ENTERED!\n";
	} break;
	case WM_POINTERLEAVE: {
		auto id = GET_POINTERID_WPARAM(wparam);
		POINTER_INPUT_TYPE type;
		if (!GetPointerType(id, &type)) {
			std::cout << "SOMETHING FAILED!" << GetLastError() << '\n';
		}
		std::cout << util::ptype_name(type) << " LEFT!\n";
	} break;
	case WM_POINTERDOWN: {
		Tablet::pointerid_t id = GET_POINTERID_WPARAM(wparam);
		POINTER_INPUT_TYPE type;
		if (!GetPointerType(id, &type)) {
			std::cout << "SOMETHING FAILED!" << GetLastError() << '\n';
		}
		if (type == PT_PEN) {
			POINTER_PEN_INFO info;
			GetPointerPenInfo(id, &info);
			std::cout << "X: " << info.pointerInfo.ptPixelLocation.x
					  << " | Y: " << info.pointerInfo.ptPixelLocation.y << '\n';
		}
	} break;
	case WM_POINTERUP: {
		Tablet::pointerid_t id = GET_POINTERID_WPARAM(wparam);
		POINTER_INPUT_TYPE type;
		if (!GetPointerType(id, &type)) {
			std::cout << "SOMETHING FAILED!" << GetLastError() << '\n';
		}
		if (type == PT_PEN) {
			POINTER_PEN_INFO info;
			GetPointerPenInfo(id, &info);
			std::cout << "X: " << info.pointerInfo.ptPixelLocation.x
					  << " | Y: " << info.pointerInfo.ptPixelLocation.y << '\n';
		}
	} break;
	default:
		return CallWindowProc(next, hwnd, umsg, wparam, lparam);
	}
	return 0;
}

} // namespace npp::tablet
