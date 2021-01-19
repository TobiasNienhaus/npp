//
// Created by Tobias on 1/18/2021.
//

#ifndef NPP_D2DBITMAPADAPTER_HPP
#define NPP_D2DBITMAPADAPTER_HPP

#include <concepts>

#include "../util/d2d_headers.hpp"

#include "D2DDrawer.hpp"
#include "direct2d_helpers.hpp"

template<typename drawer_t>
requires std::is_base_of_v<D2DDrawer, drawer_t> class D2DBitmapAdapter :
	public D2DDrawer {
private:
	drawer_t m_drawer;
	CComPtr<ID2D1BitmapRenderTarget> m_bitmapTarget;

protected:
	void draw() override {
		if (m_drawer.paint_event()) {
			ID2D1Bitmap *bitmap;
			auto hr = m_bitmapTarget->GetBitmap(&bitmap);
			if (SUCCEEDED(hr)) {
				auto destSize{get_render_target()->GetSize()};
				D2D1_RECT_F destRect = D2D1::RectF(0.f, 0.f, destSize.width, destSize.height);
				auto sourceSize{bitmap->GetSize()};
				D2D1_RECT_F sourceRect = D2D1::RectF(0.f, 0.f, sourceSize.width, sourceSize.height);
				get_render_target()->DrawBitmap(
					bitmap, &destRect, 1.f,
					D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &sourceRect);
			}
			//			get_render_target().Draw
		} else {
			// TODO somehow recreate a new bitmap render target
		}
	}

	HRESULT create_device_dependent_resources() override {
		auto hr = D2DDrawer::create_device_dependent_resources();

		if (FAILED(hr)) { return hr; }

		static const D2D1_PIXEL_FORMAT format = D2D1::PixelFormat(
			DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

		hr = get_render_target()->CreateCompatibleRenderTarget(
			nullptr, nullptr, &format,
			D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, &m_bitmapTarget);

		return hr;
	}

	void discard_device_dependent_resources() override {
		npp::graphics::com_safe_release(m_bitmapTarget);
		D2DDrawer::discard_device_dependent_resources();
	}

public:
	explicit D2DBitmapAdapter(HWND hwnd) : D2DDrawer(hwnd), m_drawer{hwnd} {}

	BOOL handle_message(UINT msg, WPARAM wp, LPARAM lp) override {
		return m_drawer.handle_message(msg, wp, lp);
	}

	void set_render_target(ID2D1RenderTarget *renderTarget) override {
		D2DDrawer::set_render_target(renderTarget);
		m_drawer.set_render_target(m_bitmapTarget);
	}

	void after_draw() override {
		m_drawer.after_draw();
	}
};

#endif // NPP_D2DBITMAPADAPTER_HPP
