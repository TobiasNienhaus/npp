//
// Created by Tobias on 1/12/2021.
//

#include "ImGuiHandler.hpp"

#include <imgui.h>
#include <imgui_impl_dx10.h>
#include <imgui_impl_win32.h>
#include <iostream>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
															 UINT msg,
															 WPARAM wParam,
															 LPARAM lParam);

ImGuiHandler::ImGuiHandler(HWND window, ID3D10Device *device, ID3D10RenderTargetView *renderTarget) :
	m_window{window}, m_device{device},
	m_renderTarget{renderTarget} {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(m_window);
	ImGui_ImplDX10_Init(m_device);
}

ImGuiHandler::~ImGuiHandler() {
	ImGui_ImplDX10_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

void ImGuiHandler::frame_prep() {
	ImGui_ImplDX10_NewFrame();
	ImGui_ImplWin32_NewFrame();
}

void ImGuiHandler::frame() {
	ImGui::NewFrame();

	draw();

	m_device->OMSetRenderTargets(1, &m_renderTarget, nullptr);
	ImGui::Render();
	// TODO figure out what this does
	auto *drawData = ImGui::GetDrawData();
	ImGui_ImplDX10_RenderDrawData(drawData);
}

void ImGuiHandler::draw() {
	ImGui::BeginMainMenuBar();
	if(ImGui::BeginMenu("File")) {
		if(ImGui::MenuItem("Test...")) {
			std::cout << "Hello there!\n";
		}
		ImGui::EndMenu();
	}
	if(ImGui::BeginMenu("Settings")) {
		if(ImGui::MenuItem("Print something", nullptr)) {
			std::cout << "Printed something!\n";
		}
		ImGui::Separator();
		if(ImGui::MenuItem("Print something more", nullptr)) {
			std::cout << "Printed something more!\n";
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	ImGui::ShowDemoWindow();
}

LRESULT ImGuiHandler::handle_message(UINT msg, WPARAM wp, LPARAM lp) {
	if (ImGui_ImplWin32_WndProcHandler(m_window, msg, wp, lp)) return true;

	return 0;
}

void ImGuiHandler::set_render_target(ID3D10RenderTargetView *newTarget) {
	std::cout << "Resize\n";
	m_renderTarget = newTarget;
}
