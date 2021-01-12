//
// Created by Tobias on 1/6/2021.
//

#ifndef NPP_TABLETWIN_HPP
#define NPP_TABLETWIN_HPP

#include "D2DWindow.hpp"
#include "../tab/Tablet.hpp"

class TabletWin : public D2DWindow {
public:
	[[nodiscard]] LPCWSTR class_name() const override;
	LRESULT handle_message(UINT msg, WPARAM wp, LPARAM lp) override;

private:
	npp::Tablet m_tablet;
	npp::Tablet::PointData m_lastPoint;

	IDWriteTextFormat *m_textFormat{};
	ID2D1SolidColorBrush *m_brush{};

	bool m_clearFlag{true};

protected:
	void draw() override;

	BOOL on_valid_context_creation() override;

	HRESULT create_device_independent_resources() override;
	void discard_device_independent_resources() override;
	HRESULT create_device_dependent_resources() override;
	void discard_device_dependent_resources() override;

};

#endif // NPP_TABLETWIN_HPP
