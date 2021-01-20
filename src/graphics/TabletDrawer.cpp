//
// Created by Tobias on 1/18/2021.
//

#include "TabletDrawer.hpp"
#include "direct2d_helpers.hpp"

#include <imgui.h>

#include "../globals.hpp"

TabletDrawer::TabletDrawer(HWND hwnd) :
	D2DDrawer(hwnd),
	m_tablet{hwnd},
	m_lastPoint{},
	m_brush{nullptr} {}

BOOL TabletDrawer::handle_message(UINT msg, WPARAM wp, LPARAM) {
	auto tabEvent = m_tablet.handle_event(msg, wp);
	if (tabEvent != npp::Tablet::Event::UNHANDLED) {
		if (tabEvent == npp::Tablet::Event::DOWN) {
			// End last line and start a new one
			// (Maybe push back m_lastPoint?)
			m_lastPoint = {};
		}
		return TRUE;
	}
	return FALSE;
}

void TabletDrawer::draw() {
	get_render_target()->SetTransform(D2D1::Matrix3x2F::Identity());

	if (Globals::clear_drawing_surface()) {
		constexpr D2D1_COLOR_F clearCol{0.f, 0.f, 0.f, 0.f};
		get_render_target()->Clear(clearCol);
		Globals::clear_drawing_surface() = false;
	}
	if (Globals::redraw_drawing_surface()) {
		for (const auto &line : m_tablet.get_all_lines()) {
			npp::tablet_types::PointData lastPoint{};
			for (const auto &p : line) {
				if (lastPoint.valid) {
					auto a = D2D1::Point2F(lastPoint.x, lastPoint.y);
					auto b = D2D1::Point2F(p.x, p.y);
					get_render_target()->DrawLine(a, b, m_brush,
												  p.pressure * 2.5f, nullptr);
				}
				lastPoint = p;
			}
		}
		Globals::redraw_drawing_surface() = false;
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

HRESULT TabletDrawer::create_device_dependent_resources() {
	auto hr = D2DDrawer::create_device_dependent_resources();
	if (SUCCEEDED(hr)) {
		constexpr D2D1_COLOR_F col{1.0f, 1.0f, 1.0f, 1.0f};
		hr = get_render_target()->CreateSolidColorBrush(col, &m_brush);
	}
	return hr;
}

void TabletDrawer::discard_device_dependent_resources() {
	npp::graphics::com_safe_release(m_brush);
	D2DDrawer::discard_device_dependent_resources();
}

void TabletDrawer::after_draw() {
	auto pos = m_tablet.get_pen_pos();
	if (pos.has_value()) {
		auto &io = ImGui::GetIO();
		io.MousePos = {pos->x, pos->y};
	}
}
