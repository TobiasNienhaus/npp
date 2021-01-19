//
// Created by Tobias on 1/12/2021.
//

#include "D3D10Window.hpp"
#include <iostream>
#include <memory>

#include <imgui.h>

#include "direct2d_helpers.hpp"

#include "../files/filehandling.hpp"
#include "../globals.hpp"

void ErrorDescription(HRESULT hr) {
	if (FACILITY_WINDOWS == HRESULT_FACILITY(hr)) hr = HRESULT_CODE(hr);
	TCHAR *szErrMsg;

	if (FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
			hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&szErrMsg, 0,
			NULL) != 0) {
		_tprintf(TEXT("%s"), szErrMsg);
		LocalFree(szErrMsg);
	} else
		_tprintf(TEXT("[Could not find a description for error # %#x.]\n"), hr);
}

BOOL D3D10Window::on_valid_context_creation() {
	// FIXME make returning boolean possible on fail

	// TODO Understand all flags
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height = 0;
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	// FIXME No fixed refresh rate
	//	desc.BufferDesc.RefreshRate.Numerator = 60;
	//	desc.BufferDesc.RefreshRate.Denominator = 1;
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
	flags |= D3D10_CREATE_DEVICE_BGRA_SUPPORT;

	static const D3D10_FEATURE_LEVEL1 levelAttempts[] = {
		D3D10_FEATURE_LEVEL_10_1,
		D3D10_FEATURE_LEVEL_10_0,
		D3D10_FEATURE_LEVEL_9_3,
		D3D10_FEATURE_LEVEL_9_2,
		D3D10_FEATURE_LEVEL_9_1,
	};

	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2dFactory);
	if (FAILED(hr)) { return FALSE; }

	int counter{};
	for (auto levelAttempt : levelAttempts) {
		ID3D10Device1 *device = nullptr;
		hr = D3D10CreateDevice1(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr,
								flags, levelAttempt, D3D10_1_SDK_VERSION,
								&device);
		if (SUCCEEDED(hr)) {
			m_device = device;
			device = nullptr;
			std::cout << "At level " << counter << '\n';
			break;
		}
		++counter;
	}

	if (FAILED(hr)) { return FALSE; }

	IDXGIDevice1 *gxdiDevice;
	hr = m_device->QueryInterface(&gxdiDevice);

	if (FAILED(hr)) { return FALSE; }

	IDXGIAdapter *gxdiAdapter;
	hr = gxdiDevice->GetAdapter(&gxdiAdapter);

	if (FAILED(hr)) { return FALSE; }

	IDXGIFactory *gxdiFactory;
	hr = gxdiAdapter->GetParent(IID_PPV_ARGS(&gxdiFactory));

	if (FAILED(hr)) { return FALSE; }

	hr = gxdiFactory->CreateSwapChain(m_device, &desc, &m_swapChain);

	if (FAILED(hr)) { return FALSE; }

	hr = create_render_target();
	if (FAILED(hr)) {
		ErrorDescription(hr);
		return FALSE;
	}

	hr = create_d2d_render_target();
	if (FAILED(hr)) {
		ErrorDescription(hr);
		return FALSE;
	}

	m_imgui = std::make_unique<ImGuiHandler>(get_window(), m_device,
											 m_mainRenderTargetView);
	m_drawer = std::make_unique<D2DBitmapAdapter<TabletDrawer>>(get_window());
	m_drawer->set_render_target(m_d2dRenderTarget);
	hr = m_drawer->initialize();
	return FAILED(hr) ? FALSE : TRUE;
}

LRESULT D3D10Window::handle_message(UINT msg, WPARAM wp, LPARAM lp) {
	if (m_imgui && m_imgui->handle_message(msg, wp, lp)) { return true; }
	if (m_drawer && m_drawer->handle_message(msg, wp, lp)) { return true; }
	//	if (msg != WM_PAINT && msg != WM_SETCURSOR) {
	//	std::cout << "MSG: " << msg << '\n';
	//	}
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(get_window());
		// Else: User canceled. Do nothing.
		return 0;
	case WM_SIZE: {
		if (m_device != nullptr && wp != SIZE_MINIMIZED) {
			if (m_mainRenderTargetView) {
				m_mainRenderTargetView.Release();
				m_mainRenderTargetView = nullptr;
			}
			if (m_d2dRenderTarget) {
				m_d2dRenderTarget.Release();
				m_d2dRenderTarget = nullptr;
			}

			auto hr = m_swapChain->ResizeBuffers(
				0, (UINT)LOWORD(lp), (UINT)HIWORD(lp), DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr)) {
				ErrorDescription(hr);
				return 0;
			}

			hr = create_render_target();
			if (hr == ERROR_PATH_NOT_FOUND) { std::cerr << "WHat?\n"; }
			if (FAILED(hr)) {
				ErrorDescription(hr);
				return 0;
			}
			hr = create_d2d_render_target();
			if (FAILED(hr)) {
				ErrorDescription(hr);
				return 0;
			}
		}
	}
		return 0;
	case WM_PAINT: {
		ImVec4 clearCol{0.f, 0.f, 0.f, 0.f};
		m_device->ClearRenderTargetView(m_mainRenderTargetView,
										(float *)&clearCol);
		if (m_drawer && !m_drawer->paint_event()) {
			create_d2d_render_target();
			std::cerr << "?\n";
		}
		m_imgui->frame_prep();
		m_drawer->after_draw();
		m_imgui->frame();
		m_swapChain->Present(1, 0);

		return 1;
	}
	default:
		return DefWindowProc(get_window(), msg, wp, lp);
	}
}

LPCWSTR D3D10Window::class_name() const {
	return L"DirectX 10 Context";
}

HRESULT D3D10Window::create_render_target() {
	CComPtr<ID3D10Texture2D> backBuffer;
	auto hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr)) {
		ErrorDescription(hr);
		return hr;
	}
	hr = m_device->CreateRenderTargetView(backBuffer, nullptr,
										  &m_mainRenderTargetView);
	if (FAILED(hr) || hr == ERROR_PATH_NOT_FOUND) {
		ErrorDescription(hr);
		return hr;
	}
	if (m_imgui) {
		m_imgui->set_render_target(m_mainRenderTargetView);
	} else
		return ERROR_BAD_DEVICE;
	return S_OK;
}

HRESULT D3D10Window::create_d2d_render_target(bool debug) {
	CComPtr<IDXGISurface> surface; // Maybe this has to be a member and fill in
								   // create_render_target

	auto dpi = GetDpiForWindow(get_window());

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpi, dpi);
	//
	//
	//	constexpr D2D1_RENDER_TARGET_PROPERTIES props{
	//		.type = D2D1_RENDER_TARGET_TYPE_DEFAULT,
	//		.pixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
	//		.dpiX = 0.f,
	//		.dpiY = 0.f,
	//		.usage = D2D1_RENDER_TARGET_USAGE_NONE,
	//		.minLevel = D2D1_FEATURE_LEVEL_10};
	auto hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
	if (FAILED(hr)) {
		ErrorDescription(hr);
		return hr;
	}
	if (!debug) {
		hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(surface, props,
														 &m_d2dRenderTarget);
	}
	if (FAILED(hr)) {
		std::cerr << "!\n";
		ErrorDescription(hr);
		hr = hr;
	}
	if (m_drawer) { m_drawer->set_render_target(m_d2dRenderTarget); }
	return hr;
}
