cmake_minimum_required(VERSION 3.17)
project(npp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)

add_executable(npp
			   src/main.cpp
			   src/tab/tablet_handling.cpp
			   src/tab/tablet_handling.hpp
			   src/tab/Tablet.cpp
			   src/tab/Tablet.hpp
			   src/tab/tablet_util.cpp
			   src/tab/tablet_util.hpp
			   src/tab/tablet_props.cpp
			   src/tab/tablet_props.hpp
			   )

target_include_directories(npp PRIVATE
						   external/glfw/include
						   external/glad/include
						   external/imgui
						   external/magic_enum
						   )

target_link_libraries(npp glad glfw ${GLFW_LIBRARIES} imgui_glfw)