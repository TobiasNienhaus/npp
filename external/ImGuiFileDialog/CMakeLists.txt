cmake_minimum_required(VERSION 3.17)
project(npp)

set(CMAKE_CXX_STANDARD 20)

add_library(ImGuiFileDialog
			ImGuiFileDialog.cpp
			)

target_include_directories(ImGuiFileDialog PRIVATE ../imgui)

target_link_libraries(ImGuiFileDialog PRIVATE imgui_win32)
