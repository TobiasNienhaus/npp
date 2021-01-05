//
// Created by Tobias on 1/5/2021.
//

#ifndef NPP_D2DWINDOW_HPP
#define NPP_D2DWINDOW_HPP

#include "Win32Context.hpp"

#include <d2d1.h>
#pragma comment(lib, "d2d1")

class D2DWindow : public Win32Context<D2DWindow> {
private:
	ID2D1Factory *m_factory;
	ID2D1HwndRenderTarget *m_renderTarget;
	ID2D1SolidColorBrush *m_brush;
	D2D1_ELLIPSE m_ellipse;

	void calculate_layout();
	HRESULT create_graphics_resources();
	void discard_graphics_resources();
	void on_paint();
	void resize();

public:
	D2DWindow();

	LRESULT handle_message(UINT msg, WPARAM wp, LPARAM lp) override;
	[[nodiscard]] LPCWSTR class_name() const override;
};

#endif // NPP_D2DWINDOW_HPP
