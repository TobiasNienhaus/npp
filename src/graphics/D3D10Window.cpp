//
// Created by Tobias on 1/12/2021.
//

#include "D3D10Window.hpp"
#include <iostream>

#include "direct2d_helpers.hpp"

BOOL D3D10Window::on_valid_context_creation() {
	// FIXME make returning boolean possible on fail

	// TODO Understand all flags
	DXGI_SWAP_CHAIN_DESC desc;
	desc.BufferCount = 2;
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height = 0;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// FIXME No fixed refresh rate
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = get_window();
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// TODO check for fail
	UINT flags = 0;
	flags |= D3D10_CREATE_DEVICE_DEBUG;
	if (FAILED(D3D10CreateDeviceAndSwapChain(
			nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, flags,
			D3D10_SDK_VERSION, &desc, &m_swapChain, &m_device))) {
		return FALSE;
	}

	create_render_target();

	m_imgui = std::make_unique<ImGuiHandler>(get_window(), m_device, m_mainRenderTargetView);
	return TRUE;
}

LRESULT D3D10Window::handle_message(UINT msg, WPARAM wp, LPARAM lp) {
	if(m_imgui && m_imgui->handle_message(msg, wp, lp)) {
		return true;
	}
	switch(msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(get_window());
		// Else: User canceled. Do nothing.
		return 0;
	case WM_SIZE: {
		if (m_device != nullptr && wp != SIZE_MINIMIZED) {
			if(m_mainRenderTargetView != nullptr) {
				m_mainRenderTargetView.Release();
				m_mainRenderTargetView = nullptr;
			}
			m_swapChain->ResizeBuffers(0, (UINT)LOWORD(lp),
										(UINT)HIWORD(lp),
										DXGI_FORMAT_UNKNOWN, 0);
			create_render_target();
		}
	} return 0;
	case WM_PAINT:
		m_imgui->frame();
		m_swapChain->Present(0, 0);
		return 0;
	default:
		return DefWindowProc(get_window(), msg, wp, lp);
	}
}

LPCWSTR D3D10Window::class_name() const {
	return L"DirectX 10 Context";
}

void D3D10Window::create_render_target() {
	CComPtr<ID3D10Texture2D> backBuffer;
	m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	m_device->CreateRenderTargetView(backBuffer, nullptr,
									 &m_mainRenderTargetView);
	if(m_imgui) { m_imgui->set_render_target(m_mainRenderTargetView); }
}
