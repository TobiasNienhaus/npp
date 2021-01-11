//
// Created by Tobias on 1/5/2021.
//

#include "D2DWindow.hpp"

#include "direct2d_helpers.hpp"

LRESULT D2DWindow::handle_message(UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_CREATE:
		return on_create();
	case WM_DESTROY:
		return on_destroy();
	case WM_PAINT:
		on_paint();
		return 0;
	case WM_SIZE:
		return on_resize();
	case WM_CLOSE:
		return on_close();
	default:
		return DefWindowProc(get_window(), msg, wp, lp);
	}
}

LPCWSTR D2DWindow::class_name() const {
	return L"npp Direct2D context";
}

D2DWindow::D2DWindow() : m_factory{nullptr}, m_renderTarget{nullptr} {}

HRESULT D2DWindow::create_device_dependent_resources() {
	HRESULT hr = S_OK;

	if (!m_renderTarget) {
		RECT rc;
		GetClientRect(get_window(), &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = m_factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(get_window(), size),
			&m_renderTarget);

		if (SUCCEEDED(hr)) {}
	}

	return hr;
}

void D2DWindow::discard_device_dependent_resources() {
	npp::graphics::com_safe_release(&m_renderTarget);
}

void D2DWindow::on_paint() {
	HRESULT hr = create_device_dependent_resources();
	if (SUCCEEDED(hr)) {
		PAINTSTRUCT ps;
		BeginPaint(get_window(), &ps);

		m_renderTarget->BeginDraw();

		hr = m_renderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
			discard_device_dependent_resources();
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
		InvalidateRect(get_window(), nullptr, FALSE);
	}
}

LRESULT D2DWindow::on_create() {
	if (FAILED(
			D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory))) {
		return -1;
	}
	return 0;
}

LRESULT D2DWindow::on_destroy() {
	discard_device_dependent_resources();
	npp::graphics::com_safe_release(&m_factory);
	PostQuitMessage(0);
	return 0;
}

LRESULT D2DWindow::on_resize() {
	resize();
	return 0;
}

LRESULT D2DWindow::on_close() {
	if (MessageBox(get_window(), L"Really quit?", L"My application",
				   MB_OKCANCEL) == IDOK) {
		DestroyWindow(get_window());
	}
	// Else: User canceled. Do nothing.
	return 0;
}
