//
// Created by Tobias on 12/14/2020.
//

#ifndef NPP_TABLET_HPP
#define NPP_TABLET_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <queue>

namespace npp {

class Tablet {
public:
	Tablet();

	using pointerid_t = unsigned short;

	static constexpr UINT32 s_maxProperties = 128;

	struct PointData {
		bool valid{false};
		float x{};
		float y{};
		float pressure{};
	};

	struct Property {
		bool initialized;

		USHORT usagePageId;
		USHORT usageId;

		INT32 min;
		INT32 max;

		[[nodiscard]] float normalize(INT32 val, bool shouldClamp = false) const;
	};

	void pen_enter(pointerid_t id);
	void pen_exit(pointerid_t id);
	void pen_down(pointerid_t id);
	void pen_up(pointerid_t id);

	PointData get_next();
	template<size_t count>
	std::array<PointData, count> get_next_n() {
		std::array<PointData, count> ret;
		for (int i = 0; i < count; ++i) {
			ret[i] = get_next();
		}
		return ret;
	}
	std::vector<PointData> get_all();

	void update();

private:
	bool m_valid;
	bool m_down;
	pointerid_t m_pointer;
	// maybe use vector, if it has push (back) and pop (front)
	std::queue<PointData> m_points;

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
};

} // namespace npp

#endif // NPP_TABLET_HPP
