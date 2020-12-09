cmake_minimum_required(VERSION 3.17)
project(npp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)

add_executable(npp
			   src/main.cpp
			   )

target_include_directories(npp PRIVATE
						   external/glfw/include
						   external/glad/include
						   external/imgui
						   )

target_link_libraries(npp glad glfw ${GLFW_LIBRARIES} imgui_glfw)