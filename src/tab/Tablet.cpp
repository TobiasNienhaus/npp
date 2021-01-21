//
// Created by Tobias on 12/14/2020.
//

#include "Tablet.hpp"

#include <iostream>

#include <imgui.h>

#include <nlohmann/json.hpp>

#include "tablet_props.hpp"
#include "tablet_util.hpp"

#include "../files/filehandling.hpp"

namespace npp {

Tablet::Tablet(HWND hwnd) :
	m_openCallbackId{897798644656454},
	m_saveCallbackId{779898789798798},
	m_valid{false},
	m_down{false},
	m_pointer{0},
	m_points{},
	m_penInFrame{false},
	m_windowHandle{hwnd},
	m_pressure{s_pressureDefault},
	m_tiltX{s_tiltXDefault},
	m_tiltY{s_tiltYDefault} {
	m_openCallbackId = Globals::register_open_callback([this] {
		open_callback();
	});
	m_saveCallbackId = Globals::register_save_callback([this] {
		save_callback();
	});
}

Tablet::~Tablet() {
	Globals::deregister_open_callback(m_openCallbackId);
	Globals::deregister_save_callback(m_saveCallbackId);
}

void Tablet::read_properties() {
	POINTER_INFO info;
	if (!GetPointerInfo(m_pointer, &info)) {
		std::cout << "could not get pointer info for pointer " << m_pointer
				  << '\n';
		m_pointer = 0;
		m_valid = false;
		m_down = false;
		return;
	}

	UINT32 propertyCount = s_maxProperties;
	POINTER_DEVICE_PROPERTY properties[s_maxProperties];

	if (!GetPointerDeviceProperties(info.sourceDevice, &propertyCount,
									properties)) {
		std::cout << "Failed reading props " << GetLastError() << '\n';
		std::cout << "Read " << propertyCount << " properties\n";
		set_properties_to_default();
		return;
	}
	for (int i = 0; i < propertyCount; ++i) {
		auto &prop = properties[i];
		switch (props::to_usage(prop.usagePageId, prop.usageId)) {
		case props::Usage::TIP_PRESSURE:
			std::cout << "PRESS MIN: " << prop.logicalMin
					  << ", PRESS MAX: " << prop.logicalMax << '\n';
			std::cout << "PRESS PHYS MIN: " << prop.physicalMin
					  << ", PRESS PHYS MAX: " << prop.physicalMax << '\n';
			break;
		case props::Usage::BARREL_PRESSURE:
			std::cout << "BARREL MIN: " << prop.logicalMin
					  << ", BARREL MAX: " << prop.logicalMax << '\n';
			m_pressure = {.initialized = true,
						  .usagePageId = prop.usagePageId,
						  .usageId = prop.usageId,
						  .min = prop.logicalMin,
						  .max = prop.logicalMax};
			break;
		case props::Usage::X_TILT:
			m_tiltX = {.initialized = true,
					   .usagePageId = prop.usagePageId,
					   .usageId = prop.usageId,
					   .min = prop.logicalMin,
					   .max = prop.logicalMax};
			break;
		case props::Usage::Y_TILT:
			m_tiltY = {.initialized = true,
					   .usagePageId = prop.usagePageId,
					   .usageId = prop.usageId,
					   .min = prop.logicalMin,
					   .max = prop.logicalMax};
			break;
		default:
			break;
		}
	}
	//	set_properties_to_default();
}

void Tablet::clear_props() {
	m_pressure = {};
	m_tiltX = {};
	m_tiltY = {};
}

void Tablet::set_properties_to_default() {
	if (!m_pressure.initialized) { m_pressure = s_pressureDefault; }
	if (!m_tiltX.initialized) { m_pressure = s_tiltXDefault; }
	if (!m_tiltY.initialized) { m_tiltY = s_tiltYDefault; }
}

void Tablet::pen_enter(Tablet::pointerid_t id) {
	m_valid = false;
	if (util::id_to_type(id) == util::PointerType::PEN) {
		m_pointer = id;

		// get source device
		POINTER_INFO info;
		if (!GetPointerInfo(m_pointer, &info)) {
			std::cerr << "could not get pointer info for pointer " << m_pointer
					  << '\n';
			m_pointer = 0;
			return;
		}

		read_properties();
		m_valid = true;
		m_penInFrame = true;
		std::cout << "Pointer " << m_pointer << " entered!\n";
	} else {
		// TODO some error handling
	}
}

void Tablet::pen_exit(Tablet::pointerid_t id) {
	if (id == m_pointer) {
		m_down = false;
		clear_props();
		m_valid = false;
		m_pointer = 0;
		m_penInFrame = false;
	}
}

void Tablet::update() {
	if (!m_valid || util::id_to_type(m_pointer) != util::PointerType::PEN) {
		return;
	} else if (!m_down) {
		POINTER_PEN_INFO info;
		if (!GetPointerPenInfo(m_pointer, &info)) {
			std::cout << "Could not get pointer info\n";
		} else {
			auto p{info.pointerInfo.ptPixelLocation};
			if (ScreenToClient(info.pointerInfo.hwndTarget, &p)) {
				m_lastPenPos = {false, static_cast<float>(p.x),
								static_cast<float>(p.y), 0.f};
			} else {
				std::cout << "Point could not be converted to client space\n";
			}
		}
	} else {
		POINTER_PEN_INFO info[16];
		UINT32 count = 16;
		if (!GetPointerPenInfoHistory(m_pointer, &count, info)) {
			std::cout << "Could not get pointer history!" << GetLastError()
					  << '\n';
		} else {
			for (unsigned int i = count; i-- > 0;) {
				auto p{info[i].pointerInfo.ptPixelLocation};
				if (ScreenToClient(info[i].pointerInfo.hwndTarget, &p)) {
					point_data_t pd{true, static_cast<float>(p.x),
									static_cast<float>(p.y),
									m_pressure.normalize(info[i].pressure)};
					m_points.push(pd);
					m_lines.back().push(pd);
					Globals::has_unsaved_changes() = true;
				} else {
					std::cout
						<< "Point could not be converted to client space\n";
				}
			}
		}
	}
}

void Tablet::pen_down(pointerid_t id) {
	if (id == m_pointer) {
		if (auto &io = ImGui::GetIO(); io.WantCaptureMouse) {
			io.MouseDown[0] = true;
		} else {
			m_down = true;
			m_lines.emplace_back();
			Globals::has_unsaved_changes() = true;
		}
	}
}

void Tablet::pen_up(pointerid_t id) {
	if (id == m_pointer) {
		if (auto &io = ImGui::GetIO(); io.WantCaptureMouse) {
			io.MouseDown[0] = false;
		} else {
			m_down = false;
		}
	}
}

Tablet::point_data_t Tablet::get_next() {
	if (m_points.empty()) {
		return {};
	} else {
		auto p = point_data_t{m_points.front()};
		m_points.pop();
		return p;
	}
}

std::vector<Tablet::point_data_t> Tablet::get_all() {
	auto p = get_next();
	if (!p.valid) { return {}; }
	std::vector<point_data_t> ret;
	while (p.valid) {
		ret.push_back(p);
		p = get_next();
	}
	return ret;
}

Tablet::Event Tablet::handle_event(UINT msg, WPARAM wp) {
	auto id = GET_POINTERID_WPARAM(wp);
	switch (msg) {
	case WM_POINTERENTER: {
		pen_enter(id);
	}
		return Event::ENTER;
	case WM_POINTERLEAVE: {
		pen_exit(id);
	}
		return Event::LEAVE;
	case WM_POINTERDOWN: {
		pen_down(id);
	}
		return Event::DOWN;
	case WM_POINTERUP: {
		pen_up(id);
	}
		return Event::UP;
	case WM_POINTERUPDATE:
		if (id == m_pointer) { update(); }
		return Event::UPDATE;
	default:
		return Event::UNHANDLED;
	}
}

void Tablet::set_hwnd(HWND hwnd) {
	m_windowHandle = hwnd;
}
std::optional<Tablet::point_data_t> Tablet::get_pen_pos() {
	if (m_penInFrame) {
		return m_lastPenPos;
	} else {
		return {};
	}
}

const std::vector<Tablet::line_t> &Tablet::get_all_lines() {
	return m_lines;
}

void Tablet::save_callback() {
	std::cout << "Save to " << Globals::filename().value_or("ERROR") << '\n';
	npp::file::save_data(Globals::filename().value(), get_all_lines());
}

void Tablet::open_callback() {
	if (Globals::file_to_load().has_value()) {
		auto vec{npp::file::load_data(Globals::file_to_load().value())};
		if (!vec.empty()) {
			Globals::filename() = Globals::file_to_load().value();
			Globals::file_to_load() = {};
			m_lines = vec;
		}
	} else {
		m_lines.clear();
	}
	Globals::clear_drawing_surface() = true;
	Globals::redraw_drawing_surface() = true;
}

float Tablet::Property::normalize(INT32 val, bool shouldClamp) const {
	auto ret =
		static_cast<float>((static_cast<double>(val) - min) / (max - min));
	if (ret == std::numeric_limits<decltype(ret)>::infinity()) {
		std::cerr << "wut??? " << min << ", " << max << '\n';
	}
	if (shouldClamp) {
		return std::clamp(ret, 0.f, 1.f);
	} else {
		return ret;
	}
}

namespace tablet_types {
void Line::push(PointData pd) {
	m_points.push_back(pd);
}
const Line::container_t &Line::points() const {
	return m_points;
}
Line::container_t &Line::points() {
	return m_points;
}
Line::cit_t Line::begin() const {
	return m_points.cbegin();
}
Line::cit_t Line::end() const {
	return m_points.cend();
}
Line::it_t Line::begin() {
	return m_points.begin();
}
Line::it_t Line::end() {
	return m_points.end();
}
} // namespace tablet_types
} // namespace npp
