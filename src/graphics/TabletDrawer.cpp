//
// Created by Tobias on 1/18/2021.
//

#include "TabletDrawer.hpp"
#include "direct2d_helpers.hpp"
TabletDrawer::TabletDrawer(HWND hwnd) :
	D2DDrawer(hwnd),
	m_tablet{hwnd},
	m_lastPoint{},
	m_lines{},
	m_brush{nullptr} {}

BOOL TabletDrawer::handle_message(UINT msg, WPARAM wp, LPARAM) {
	auto tabEvent = m_tablet.handle_event(msg, wp);
	if (tabEvent != npp::Tablet::Event::UNHANDLED) {
		if (tabEvent == npp::Tablet::Event::DOWN) {
			// End last line and start a new one
			// (Maybe push back m_lastPoint?)
			m_lastPoint = {};
			m_lines.emplace_back();
		}
		return TRUE;
	}
	return FALSE;
}

void TabletDrawer::draw() {
	get_render_target()->SetTransform(D2D1::Matrix3x2F::Identity());

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

HRESULT TabletDrawer::create_device_dependent_resources() {
	auto hr = D2DDrawer::create_device_dependent_resources();
	if(SUCCEEDED(hr)) {
		constexpr D2D1_COLOR_F col{1.0f, 1.0f, 1.0f, 1.0f};
		hr = get_render_target()->CreateSolidColorBrush(col, &m_brush);
	}
	return hr;
}

void TabletDrawer::discard_device_dependent_resources() {
	npp::graphics::com_safe_release(m_brush);
	D2DDrawer::discard_device_dependent_resources();
}
