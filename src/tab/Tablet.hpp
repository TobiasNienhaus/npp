//
// Created by Tobias on 12/14/2020.
//

#ifndef NPP_TABLET_HPP
#define NPP_TABLET_HPP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <queue>
#include <optional>

#include <nlohmann/json_fwd.hpp>

#include "../globals.hpp"

namespace npp {

namespace tablet_types {
struct PointData {
	bool valid{false};
	float x{};
	float y{};
	float pressure{};
};

class Line {
public:
	using container_t = std::vector<PointData>;
	using cit_t = container_t::const_iterator;
	using it_t = container_t::iterator;

private:
	container_t m_points;

public:
	Line() = default;
	void push(PointData pd);
	[[nodiscard]] const container_t &points() const;
	container_t &points();

	[[nodiscard]] cit_t begin() const;
	[[nodiscard]] it_t begin();
	[[nodiscard]] cit_t end() const;
	[[nodiscard]] it_t end();
};

}

class Tablet {
private:
	using point_data_t = tablet_types::PointData;
	using line_t = tablet_types::Line;

public:
	enum class Event {
		LEAVE,
		ENTER,
		DOWN,
		UP,
		UPDATE,
		UNHANDLED
	};

public:
	explicit Tablet(HWND hwnd = nullptr);
	~Tablet();

	using pointerid_t = unsigned short;

	static constexpr UINT32 s_maxProperties = 512;

	struct Property {
		bool initialized;

		USHORT usagePageId;
		USHORT usageId;

		INT32 min;
		INT32 max;

		[[nodiscard]] float normalize(INT32 val, bool shouldClamp = false) const;
	};

	Event handle_event(UINT msg, WPARAM wp);

	void set_hwnd(HWND hwnd);

	void pen_enter(pointerid_t id);
	void pen_exit(pointerid_t id);
	void pen_down(pointerid_t id);
	void pen_up(pointerid_t id);

	point_data_t get_next();
	template<size_t count>
	std::array<point_data_t, count> get_next_n() {
		std::array<point_data_t, count> ret;
		for (int i = 0; i < count; ++i) {
			ret[i] = get_next();
		}
		return ret;
	}
	std::vector<point_data_t> get_all();

	std::optional<point_data_t> get_pen_pos();

	void update();

	const std::vector<line_t> &get_all_lines();

private:
	Globals::callback_id_t m_openCallbackId;
	Globals::callback_id_t m_saveCallbackId;

	bool m_valid;
	bool m_down;
	pointerid_t m_pointer;
	// maybe use vector, if it has push (back) and pop (front)
	std::queue<point_data_t> m_points;
	point_data_t m_lastPenPos;
	bool m_penInFrame{};

	std::vector<line_t> m_lines;

	HWND m_windowHandle;

	static constexpr Property s_pressureDefault = {
		true, 0x0, 0x0, 0, 4096
	};

	static constexpr Property s_tiltXDefault = {
		true, 0x0, 0x0, -9000, 9000
	};

	static constexpr Property s_tiltYDefault = {
		true, 0x0, 0x0, -9000, 9000
	};

	void read_properties();
	void clear_props();
	void set_properties_to_default();

	Property m_pressure;
	Property m_tiltX;
	Property m_tiltY;

	void save_callback();
	void open_callback();
};

} // namespace npp

#endif // NPP_TABLET_HPP
