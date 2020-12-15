//
// Created by Tobias on 12/14/2020.
//

#include "Tablet.hpp"

#include <iostream>

#include "tablet_props.hpp"
#include "tablet_util.hpp"

namespace npp {

Tablet::Tablet() :
	m_valid{false},
	m_down{false},
	m_pointer{0},
	m_sourceDevice{nullptr},
	m_points{},
	m_pressure{s_pressureDefault},
	m_tiltX{s_tiltXDefault},
	m_tiltY{s_tiltYDefault} {}

void Tablet::read_properties() {
	UINT32 propertyCount = s_maxProperties;
	POINTER_DEVICE_PROPERTY properties[s_maxProperties];
	if (!GetPointerDeviceProperties(m_sourceDevice, &propertyCount,
									properties)) {
		std::cerr << "Failed reading props" << GetLastError() << '\n';
		return;
	}
	for (int i = 0; i < propertyCount; ++i) {
		auto &prop = properties[i];
		switch (props::to_usage(prop.usagePageId, prop.usageId)) {
		case props::Usage::TIP_PRESSURE:
			std::cout << "PRESS MIN: " << prop.logicalMin
					  << ", PRESS MAX: " << prop.logicalMax << '\n';
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
	set_properties_to_default();
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
		if (!GetPointerInfo(id, &info)) {
			std::cerr << "could not get pointer info for pointer " << m_pointer
					  << '\n';
			m_pointer = 0;
			return;
		}
		m_sourceDevice = info.sourceDevice;

		read_properties();
		m_valid = true;
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
	}
}

void Tablet::update() {
	if (!m_valid || !m_down) {
		return;
	} else {
		if (util::id_to_type(m_pointer) == util::PointerType::PEN) {
			POINTER_PEN_INFO info[512];
			UINT32 count = 512;
			if (!GetPointerPenInfoHistory(m_pointer, &count, info)) {
				std::cout << "Could not get pointer history!" << GetLastError()
						  << '\n';
			} else {
				// TODO write into m_points
				std::cout << "COUNT: " << count << '\n';
				for (int i = 0; i < count; ++i) {
					// TODO fit to monitor
					m_points.push({true,
								   static_cast<double>(
									   info[i].pointerInfo.ptPixelLocation.x),
								   static_cast<double>(
									   info[i].pointerInfo.ptPixelLocation.y),
								   m_pressure.normalize(info[i].pressure)});
				}
			}
		}
	}
}

void Tablet::pen_down(pointerid_t id) {
	if (id == m_pointer) { m_down = true; }
}

void Tablet::pen_up(pointerid_t id) {
	if (id == m_pointer) { m_down = false; }
}

Tablet::PointData Tablet::get_next() {
	if (m_points.empty()) {
		return {};
	} else {
		auto p = PointData{m_points.front()};
		m_points.pop();
		return p;
	}
}

std::vector<Tablet::PointData> Tablet::get_all() {
	auto p = get_next();
	std::vector<PointData> ret;
	while(p.valid) {
		ret.push_back(p);
		p = get_next();
	}
	return ret;
}

double Tablet::Property::normalize(INT32 val, bool shouldClamp) const {
	double ret = (static_cast<double>(val) - min) / (max - min);
	if (shouldClamp) {
		return std::clamp(ret, 0.0, 1.0);
	} else {
		return ret;
	}
}

} // namespace npp
