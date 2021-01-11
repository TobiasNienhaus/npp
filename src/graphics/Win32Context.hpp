//
// Created by Tobias on 1/4/2021.
//

#ifndef NPP_WIN32CONTEXT_HPP
#define NPP_WIN32CONTEXT_HPP

#define UNICODE
#define _UNICODE
#define NOMINMAX

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <concepts>

template<typename derived_t>
class Win32Context {
public:
	Win32Context() : m_window{nullptr} {}

	BOOL create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle = 0,
				int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
				int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT,
				HWND hWndParent = nullptr, HMENU hMenu = nullptr) {
		WNDCLASS wc = {0};

		wc.lpfnWndProc = derived_t::s_win_proc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = class_name();

		RegisterClass(&wc);

		m_window = CreateWindowEx(dwExStyle, class_name(), lpWindowName,
								  dwStyle, x, y, nWidth, nHeight, hWndParent,
								  hMenu, GetModuleHandle(nullptr), this);
		if(valid()) {
			on_valid_context_creation();
		}
		return valid();
	}

	[[maybe_unused]] [[nodiscard]] HWND get_window() const { return m_window; }

	BOOL valid() { return (get_window() ? TRUE : FALSE); }

	void show(int nCmdShow) { ShowWindow(get_window(), nCmdShow); }

private:
	static LRESULT CALLBACK s_win_proc(HWND hwnd, UINT msg, WPARAM wp,
									   LPARAM lp) {
		derived_t *state;
		if (msg == WM_CREATE) {
			auto *cs = reinterpret_cast<CREATESTRUCT *>(lp);
			state = reinterpret_cast<derived_t *>(cs->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA,
							 reinterpret_cast<LONG_PTR>(state));

			state->m_window = hwnd;
		} else {
			state = reinterpret_cast<derived_t *>(
				GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		if (state) {
			return state->handle_message(msg, wp, lp);
		} else {
			return DefWindowProc(hwnd, msg, wp, lp);
		}
	}

public:
	virtual LRESULT handle_message(UINT msg, WPARAM wp, LPARAM lp) = 0;
	[[nodiscard]] virtual LPCWSTR class_name() const = 0;

protected:
	virtual void on_valid_context_creation() {}

private:
	HWND m_window;
};

#endif // NPP_WIN32CONTEXT_HPP
