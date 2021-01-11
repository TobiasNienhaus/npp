//
// Created by Tobias on 1/5/2021.
//

#ifndef NPP_D2DWINDOW_HPP
#define NPP_D2DWINDOW_HPP

#include "Win32Context.hpp"

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <dwrite.h>
#pragma comment(lib, "dwrite")

class D2DWindow : public Win32Context<D2DWindow> {
private:
	ID2D1Factory *m_factory;
	IDWriteFactory *m_writeFactory;
	ID2D1HwndRenderTarget *m_renderTarget;

	void on_paint();
	void resize();

protected:
	LRESULT on_create();
	LRESULT on_destroy();
	LRESULT on_resize();
	LRESULT on_close();

	ID2D1Factory *get_factory();
	IDWriteFactory *get_write_factory();
	ID2D1HwndRenderTarget *get_render_target();

	virtual HRESULT create_device_independent_resources();
	virtual void discard_device_independent_resources();
	virtual HRESULT create_device_dependent_resources();
	virtual void discard_device_dependent_resources();

public:
	D2DWindow();

	LRESULT handle_message(UINT msg, WPARAM wp, LPARAM lp) override;
	[[nodiscard]] LPCWSTR class_name() const override;
};

#endif // NPP_D2DWINDOW_HPP
