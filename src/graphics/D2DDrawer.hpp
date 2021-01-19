//
// Created by Tobias on 1/18/2021.
//

#ifndef NPP_D2DDRAWER_HPP
#define NPP_D2DDRAWER_HPP

#include "../util/win_headers.hpp"
#include "../util/d2d_headers.hpp"

#include "ID2DRenderTargetProvider.hpp"

class D2DDrawer {
private:
//	ID2DRenderTargetProvider &m_renderTargetProvider;
	ID2D1RenderTarget *m_renderTarget;
	ID2D1Factory *m_factory;

	HWND m_window;

protected:
	virtual void draw();

	virtual HRESULT create_device_dependent_resources();
	virtual void discard_device_dependent_resources();
	virtual HRESULT create_device_independent_resources();
	virtual void discard_device_independent_resources();

	ID2D1RenderTarget *get_render_target();
	ID2D1Factory *get_factory();

	HWND get_window();

public:
//	explicit D2DDrawer(ID2DRenderTargetProvider &renderTargetProvider);
	explicit D2DDrawer(HWND hwnd);

	HRESULT initialize();

	virtual void set_render_target(ID2D1RenderTarget *renderTarget);
	BOOL paint_event();
	virtual void discard_all_resources();

	virtual void after_draw();

	virtual BOOL handle_message(UINT msg, WPARAM wp, LPARAM lp);
};

#endif // NPP_D2DDRAWER_HPP
