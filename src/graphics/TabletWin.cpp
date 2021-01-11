//
// Created by Tobias on 1/6/2021.
//
#include "../util/colorspaces.hpp"
#include "../util/random.h"

#include "TabletWin.hpp"

#include <iostream>

#include "direct2d_helpers.hpp"

LPCWSTR TabletWin::class_name() const {
	return L"npp Tablet window";
}

LRESULT TabletWin::handle_message(UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_CREATE:
		return D2DWindow::on_create();
	case WM_DESTROY:
		return D2DWindow::on_destroy();
	case WM_SIZE:
		return D2DWindow::on_resize();
	case WM_CLOSE:
		return D2DWindow::on_close();
	case WM_PAINT:
		D2DWindow::on_paint();
		return 0;
	case WM_KEYDOWN:
		if (wp == 'A' || wp == 'a') {
			std::cout << "Clear\n";
			m_clearFlag = true;
			RedrawWindow(get_window(), nullptr, nullptr, RDW_INTERNALPAINT);
		} else if (wp == VK_ESCAPE) {
			D2DWindow::on_close();
		}
		return 0;
	default:
		break;
	}

	auto tabletEvent{m_tablet.handle_event(msg, wp)};

	if (tabletEvent != npp::Tablet::Event::UNHANDLED) {
		if (tabletEvent == npp::Tablet::Event::DOWN) { m_lastPoint = {}; }
		return 0;
	}

	return DefWindowProc(get_window(), msg, wp, lp);
}

void TabletWin::draw() {
	get_render_target()->SetTransform(D2D1::Matrix3x2F::Identity());

	constexpr static const WCHAR text[] = L"Hello, World!";

	if (m_clearFlag) {
		m_clearFlag = false;
		get_render_target()->Clear(D2D1::ColorF(D2D1::ColorF::White));
	}

	if (!m_lastPoint.valid) { m_lastPoint = m_tablet.get_next(); }
	bool draw = true;
	while (draw) {
		auto next = m_tablet.get_next();
		if (next.valid) {
			// TODO store points, if a complete redraw becomes necessary
			auto a = D2D1::Point2F(m_lastPoint.x, m_lastPoint.y);
			auto b = D2D1::Point2F(next.x, next.y);
			get_render_target()->DrawLine(a, b, m_brush, next.pressure * 2.5f,
										  nullptr);
			m_lastPoint = next;
		} else {
			draw = false;
		}
	}
}

HRESULT TabletWin::create_device_independent_resources() {
	auto hr = D2DWindow::create_device_independent_resources();
	if (FAILED(hr)) { return hr; }
	hr = get_write_factory()->CreateTextFormat(
		L"VERDANA", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 50,
		L"", // locale
		&m_textFormat);
	if (SUCCEEDED(hr)) {
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	return hr;
}

void TabletWin::discard_device_independent_resources() {
	npp::graphics::com_safe_release(&m_textFormat);
	D2DWindow::discard_device_independent_resources();
}

HRESULT TabletWin::create_device_dependent_resources() {
	auto hr = D2DWindow::create_device_dependent_resources();
	if (FAILED(hr)) { return hr; }
	hr = get_render_target()->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black), &m_brush);
	return hr;
}

void TabletWin::discard_device_dependent_resources() {
	D2DWindow::discard_device_dependent_resources();
}

void TabletWin::on_valid_context_creation() {
	m_tablet.set_hwnd(get_window());
}
