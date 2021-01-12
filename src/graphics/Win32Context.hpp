//
// Created by Tobias on 1/4/2021.
//

#ifndef NPP_WIN32CONTEXT_HPP
#define NPP_WIN32CONTEXT_HPP

#include "../util/win_headers.hpp"

#include <concepts>

template<typename derived_t>
class Win32Context {
public:
	Win32Context() : m_window{nullptr}, m_wndClass{0} {}
	virtual ~Win32Context() {
		DestroyWindow(m_window);
		UnregisterClass(m_wndClass.lpszClassName, m_wndClass.hInstance);
	}

	BOOL create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle = 0,
				int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
				int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT,
				HWND hWndParent = nullptr, HMENU hMenu = nullptr) {
		m_wndClass = {0};

		m_wndClass.lpfnWndProc = derived_t::s_win_proc;
		m_wndClass.hInstance = GetModuleHandle(nullptr);
		m_wndClass.lpszClassName = class_name();

		RegisterClass(&m_wndClass);

		m_window = CreateWindowEx(dwExStyle, class_name(), lpWindowName,
								  dwStyle, x, y, nWidth, nHeight, hWndParent,
								  hMenu, GetModuleHandle(nullptr), this);
		if(valid()) {
			if(!on_valid_context_creation()) {
				return FALSE;
			}
		}
		return valid();
	}

	[[maybe_unused]] [[nodiscard]] HWND get_window() const { return m_window; }

	BOOL valid() { return (get_window() ? TRUE : FALSE); }

	void show(int nCmdShow) { ShowWindow(get_window(), nCmdShow); }


protected:
	virtual LRESULT handle_message(UINT msg, WPARAM wp, LPARAM lp) = 0;
	[[nodiscard]] virtual LPCWSTR class_name() const = 0;

	virtual BOOL on_valid_context_creation() { return TRUE; }

private:
	static LRESULT CALLBACK s_win_proc(HWND hwnd, UINT msg, WPARAM wp,
									   LPARAM lp) {
		using win_t = Win32Context<derived_t>;
		win_t *state;
		if (msg == WM_CREATE) {
			auto *cs = reinterpret_cast<CREATESTRUCT *>(lp);
			state = reinterpret_cast<win_t *>(cs->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA,
							 reinterpret_cast<LONG_PTR>(state));

			state->m_window = hwnd;
		} else {
			state = reinterpret_cast<win_t *>(
				GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		if (state) {
			return state->handle_message(msg, wp, lp);
		} else {
			return DefWindowProc(hwnd, msg, wp, lp);
		}
	}

	HWND m_window;
	WNDCLASS m_wndClass;
};

#endif // NPP_WIN32CONTEXT_HPP
