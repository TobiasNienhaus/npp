//
// Created by Tobias on 1/5/2021.
//

#include "TestWindow.hpp"

LPCWSTR TestWindow::class_name() const {
	return L"TestWindow";
}

LRESULT TestWindow::handle_message(UINT msg, WPARAM wp, LPARAM lp) {
	switch(msg) {
	case WM_DESTROY: {
		PostQuitMessage(0);
	}
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(get_window(), &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
		EndPaint(get_window(), &ps);
	}
		return 0;
	case WM_CLOSE: {
		if (MessageBox(get_window(), L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(get_window());
		}
		// Else: User canceled. Do nothing.
	}
		return 0;
	default:
		return DefWindowProc(get_window(), msg, wp, lp);
	}
}
