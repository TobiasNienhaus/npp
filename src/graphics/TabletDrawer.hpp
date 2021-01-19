//
// Created by Tobias on 1/18/2021.
//

#ifndef NPP_TABLETDRAWER_HPP
#define NPP_TABLETDRAWER_HPP

#include <deque>
#include <queue>

#include "../util/win_headers.hpp"

#include "../tab/Tablet.hpp"
#include "D2DDrawer.hpp"

class TabletDrawer : public D2DDrawer {
private:

private:
	npp::Tablet m_tablet;
	npp::tablet_types::PointData m_lastPoint;
	CComPtr<ID2D1SolidColorBrush> m_brush;

	void draw() override;

	HRESULT create_device_dependent_resources() override;
	void discard_device_dependent_resources() override;

public:
	explicit TabletDrawer(HWND hwnd);
	BOOL handle_message(UINT msg, WPARAM wp, LPARAM lp) override;

	void after_draw() override;
};

#endif // NPP_TABLETDRAWER_HPP
