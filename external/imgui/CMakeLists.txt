cmake_minimum_required(VERSION 3.17)
project(npp)

set(CMAKE_CXX_STANDARD 20)

add_library(imgui_win32
			imgui.cpp
			imgui_demo.cpp
			imgui_draw.cpp
			imgui_impl_win32.cpp
			imgui_impl_dx10.cpp
			imgui_tables.cpp
			imgui_widgets.cpp
			imgui_stdlib.cpp
			)
