//
// Created by Tobias on 1/12/2021.
//

#ifndef NPP_D3D10WINDOW_HPP
#define NPP_D3D10WINDOW_HPP

#include <memory>

#include "../util/win_headers.hpp"
#include "../util/d3d10_headers.hpp"

#include "Win32Context.hpp"

#include "imgui/ImGuiHandler.hpp"

class D3D10Window : public Win32Context<D3D10Window> {
private:
	CComPtr<ID3D10Device> m_device;
	CComPtr<IDXGISwapChain> m_swapChain;
	CComPtr<ID3D10RenderTargetView> m_mainRenderTargetView;

	std::unique_ptr<ImGuiHandler> m_imgui;

	void create_render_target();

protected:
	BOOL on_valid_context_creation() override;
	LRESULT handle_message(UINT msg, WPARAM wp, LPARAM lp) override;
	[[nodiscard]] LPCWSTR class_name() const override;
};

#endif // NPP_D3D10WINDOW_HPP
