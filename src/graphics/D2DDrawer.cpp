//
// Created by Tobias on 1/18/2021.
//

#include "D2DDrawer.hpp"

#include "direct2d_helpers.hpp"

D2DDrawer::D2DDrawer(HWND hwnd) :
	m_renderTarget{nullptr},
	m_factory{nullptr},
	m_window{hwnd} {}

void D2DDrawer::draw() {}

HRESULT D2DDrawer::initialize() {
	auto hr = create_device_independent_resources();
	return hr;
}

BOOL D2DDrawer::paint_event() {
	//	HRESULT hr = create_device_dependent_resources();
	HRESULT hr{S_OK};
	if (SUCCEEDED(hr)) {
		// TODO Has to be enveloped by BeginDraw and EndDraw
		m_renderTarget->BeginDraw();

		draw();

		hr = m_renderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
			//			discard_device_dependent_resources();
			return FALSE;
		}
	}
	return TRUE;
}

void D2DDrawer::discard_all_resources() {
	discard_device_dependent_resources();
	discard_device_independent_resources();
}

HRESULT D2DDrawer::create_device_dependent_resources() {
	//	HRESULT hr{S_OK};
	//	if (!m_renderTarget) {
	//		hr = m_renderTargetProvider.get_d2d_render_target(m_factory,
	//														  &m_renderTarget);
	//	}
	//	return hr;
	return S_OK;
}

void D2DDrawer::discard_device_dependent_resources() {
	//	npp::graphics::com_safe_release(&m_renderTarget);
}

HRESULT D2DDrawer::create_device_independent_resources() {
	auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory);
	return hr;
}

void D2DDrawer::discard_device_independent_resources() {
	npp::graphics::com_safe_release(&m_factory);
}

ID2D1RenderTarget *D2DDrawer::get_render_target() {
	return m_renderTarget;
}

ID2D1Factory *D2DDrawer::get_factory() {
	return m_factory;
}

void D2DDrawer::set_render_target(ID2D1RenderTarget *renderTarget) {
	discard_device_dependent_resources();
	m_renderTarget = renderTarget;
	create_device_dependent_resources();
}

HWND D2DDrawer::get_window() {
	return m_window;
}

BOOL D2DDrawer::handle_message(UINT msg, WPARAM wp, LPARAM lp) {
	return FALSE;
}
