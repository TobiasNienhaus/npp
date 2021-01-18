//
// Created by Tobias on 1/12/2021.
//

#ifndef NPP_D3D10WINDOW_HPP
#define NPP_D3D10WINDOW_HPP

#include <memory>

#include "../util/win_headers.hpp"
#include "../util/d3d10_headers.hpp"

#include "ID2DRenderTargetProvider.hpp"
#include "D2DDrawer.hpp"

#include "Win32Context.hpp"

#include "imgui/ImGuiHandler.hpp"

class D3D10Window : public Win32Context<D3D10Window> {
private:
	CComPtr<ID3D10Device1> m_device;
	CComPtr<IDXGISwapChain> m_swapChain;
	CComPtr<ID3D10RenderTargetView> m_mainRenderTargetView;
	CComPtr<ID2D1RenderTarget> m_d2dRenderTarget;
	CComPtr<ID2D1Factory> m_d2dFactory;

	std::unique_ptr<ImGuiHandler> m_imgui;

	std::unique_ptr<D2DDrawer> m_drawer;

	HRESULT create_render_target();
	HRESULT create_d2d_render_target(bool debug = false);

protected:
	BOOL on_valid_context_creation() override;
	LRESULT handle_message(UINT msg, WPARAM wp, LPARAM lp) override;
	[[nodiscard]] LPCWSTR class_name() const override;

//	HRESULT get_d2d_render_target(ID2D1Factory *, ID2D1RenderTarget **) override;
};

#endif // NPP_D3D10WINDOW_HPP
