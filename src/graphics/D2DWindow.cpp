//
// Created by Tobias on 1/5/2021.
//

#include "D2DWindow.hpp"


template <typename T> void safe_release(T **comObj) {
	if (*comObj) {
		(*comObj)->Release();
		*comObj = nullptr;
	}
}

LRESULT D2DWindow::handle_message(UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
									 &m_factory))) {
			return -1;
		}
		return 0;
	case WM_DESTROY: {
		discard_graphics_resources();
		safe_release(&m_factory);
		PostQuitMessage(0);
	}
		return 0;
	case WM_PAINT:
		on_paint();
		return 0;
	case WM_SIZE:
		resize();
		return 0;
	case WM_CLOSE: {
		if (MessageBox(get_window(), L"Really quit?", L"My application",
					   MB_OKCANCEL) == IDOK) {
			DestroyWindow(get_window());
		}
		// Else: User canceled. Do nothing.
	}
		return 0;
	default:
		return DefWindowProc(get_window(), msg, wp, lp);
	}
}

LPCWSTR D2DWindow::class_name() const {
	return L"npp Direct2D context";
}

D2DWindow::D2DWindow() :
	m_factory{nullptr},
	m_renderTarget{nullptr},
	m_brush{nullptr},
	m_ellipse{} {}

void D2DWindow::calculate_layout() {
	if (m_renderTarget != nullptr) {
		D2D1_SIZE_F size = m_renderTarget->GetSize();
		const float x = size.width / 2;
		const float y = size.height / 2;
		m_ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), x, y);
	}
}

HRESULT D2DWindow::create_graphics_resources() {
	HRESULT hr = S_OK;

	if (!m_renderTarget) {
		RECT rc;
		GetClientRect(get_window(), &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = m_factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(get_window(), size),
			&m_renderTarget);

		if (SUCCEEDED(hr)) {
			const D2D1_COLOR_F color = D2D1::ColorF(1.f, 1.f, 0.f);
			hr = m_renderTarget->CreateSolidColorBrush(color, &m_brush);

			if (SUCCEEDED(hr)) { calculate_layout(); }
		}
	}

	return hr;
}

void D2DWindow::discard_graphics_resources() {
	safe_release(&m_renderTarget);
	safe_release(&m_brush);
	safe_release(&m_factory);
}

void D2DWindow::on_paint() {
	HRESULT hr = create_graphics_resources();
	if (SUCCEEDED(hr)) {
		PAINTSTRUCT ps;
		BeginPaint(get_window(), &ps);

		m_renderTarget->BeginDraw();

		m_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		m_renderTarget->FillEllipse(m_ellipse, m_brush);

		hr = m_renderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
			discard_graphics_resources();
		}
		EndPaint(get_window(), &ps);
	}
}

void D2DWindow::resize() {
	if (m_renderTarget) {
		RECT rc;
		GetClientRect(get_window(), &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		m_renderTarget->Resize(size);
		calculate_layout();
		InvalidateRect(get_window(), nullptr, FALSE);
	}
}
