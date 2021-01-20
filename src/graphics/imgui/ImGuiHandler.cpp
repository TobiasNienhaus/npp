//
// Created by Tobias on 1/12/2021.
//

#include "ImGuiHandler.hpp"

#include <imgui.h>
#include <imgui_impl_dx10.h>
#include <imgui_impl_win32.h>
#include <iostream>

#include "../../globals.hpp"

#include "../../files/filedialog.hpp"

#include "../../files/filehandling.hpp"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
													  WPARAM wParam,
													  LPARAM lParam);

ImGuiHandler::ImGuiHandler(HWND window, ID3D10Device *device,
						   ID3D10RenderTargetView *renderTarget) :
	m_window{window},
	m_device{device},
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
	constexpr const char *modalId = "Unsaved Changes";
	bool openModal = false;

	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Test")) {
		if(ImGui::MenuItem("New")) {
			if(Globals::has_unsaved_changes()) {
				m_nextMode = ReplacementMode::NEW;
				openModal = true;
			} else {
				Globals::file_to_load() = {};
				Globals::on_open_file();
			}
		}
		if (ImGui::MenuItem("Open")) {
			if(Globals::has_unsaved_changes()) {
				std::cout << "Unsaved Changes!\n";
				m_nextMode = ReplacementMode::OPEN;
				openModal = true;
			} else {
				npp::file::dialog::open(npp::file::dialog::Status::OPEN);
			}
		}
		if(ImGui::MenuItem("Save")) {
			if(Globals::filename().has_value()) {
				Globals::on_save_file();
			} else {
				npp::file::dialog::open(npp::file::dialog::Status::SAVE_AS);
			}
		}
		if(ImGui::MenuItem("Save as")) {
			npp::file::dialog::open(npp::file::dialog::Status::SAVE_AS);
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	if(openModal && !ImGui::IsPopupOpen(modalId)) {
		ImGui::OpenPopup(modalId);
	}

	if (ImGui::BeginPopupModal(modalId, nullptr,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		if(m_nextMode != ReplacementMode::NONE) {
			ImGui::Text("You have unsaved changes! Discard?");
			ImGui::Separator();
			if (ImGui::Button("Discard")) {
				Globals::has_unsaved_changes() = false;
				if(m_nextMode == ReplacementMode::OPEN) {
					npp::file::dialog::open(npp::file::dialog::Status::OPEN);
				} else if(m_nextMode == ReplacementMode::NEW) {
					Globals::file_to_load() = {};
					Globals::on_open_file();
				}
				ImGui::CloseCurrentPopup();
				m_nextMode = ReplacementMode::NONE;
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Save Changes")) {
				if(Globals::filename().has_value()) {
					Globals::on_save_file();
					if(m_nextMode == ReplacementMode::OPEN) {
						npp::file::dialog::open(npp::file::dialog::Status::OPEN);
					} else if (m_nextMode == ReplacementMode::NEW) {
						Globals::file_to_load() = {};
						Globals::on_open_file();
					}
				} else {
					if (m_nextMode == ReplacementMode::OPEN) {
						npp::file::dialog::open(
							npp::file::dialog::Status::SAVE_OPEN);
					} else if (m_nextMode == ReplacementMode::NEW) {
						npp::file::dialog::open(
							npp::file::dialog::Status::SAVE_AS_THEN_CLEAR);
					}
				}
				ImGui::CloseCurrentPopup();
				m_nextMode = ReplacementMode::NONE;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				Globals::file_to_load() = {};
				ImGui::CloseCurrentPopup();
				m_nextMode = ReplacementMode::NONE;
			}
		}
		ImGui::EndPopup();
	}

	npp::file::dialog::display();
}

LRESULT ImGuiHandler::handle_message(UINT msg, WPARAM wp, LPARAM lp) {
	if (ImGui_ImplWin32_WndProcHandler(m_window, msg, wp, lp)) return true;

	return 0;
}

void ImGuiHandler::set_render_target(ID3D10RenderTargetView *newTarget) {
	m_renderTarget = newTarget;
}
