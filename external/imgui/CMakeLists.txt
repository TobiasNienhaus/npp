cmake_minimum_required(VERSION 3.17)
project(npp)

set(CMAKE_CXX_STANDARD 20)

add_library(imgui_glfw
			imgui.cpp
			imgui_demo.cpp
			imgui_draw.cpp
			imgui_impl_opengl3.cpp
			imgui_impl_glfw.cpp
			imgui_tables.cpp
			imgui_widgets.cpp
			imgui_stdlib.cpp)

target_include_directories(imgui_glfw PRIVATE ../glad/include ../glfw/include)

target_link_libraries(imgui_glfw glad glfw ${GLFW_LIBRARIES})
