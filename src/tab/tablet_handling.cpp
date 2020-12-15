//
// Created by Tobias on 12/14/2020.
//

#include "tablet_handling.hpp"

#include <iostream>

#include "Tablet.hpp"
#include "tablet_util.hpp"

namespace npp {

LRESULT catch_tablet_msgs(WNDPROC next, HWND hwnd, UINT umsg, WPARAM wparam,
						  LPARAM lparam) {
	auto id = GET_POINTERID_WPARAM(wparam);
	switch (umsg) {
	case WM_POINTERENTER: {
		tablet().pen_enter(id);
	} break;
	case WM_POINTERLEAVE: {
		tablet().pen_exit(id);
	} break;
	case WM_POINTERDOWN: {
		tablet().pen_down(id);
	} break;
	case WM_POINTERUP: {
		tablet().pen_up(id);
	} break;
	default:
		return CallWindowProc(next, hwnd, umsg, wparam, lparam);
	}
	return 0;
}

Tablet &tablet() {
	// Leaking... on purpose
	static auto *instance = new Tablet();
	return *instance;
}

} // namespace npp
