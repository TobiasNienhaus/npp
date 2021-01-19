cmake_minimum_required(VERSION 3.17)
project(npp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/imgui)

add_executable(npp WIN32
			   src/main.cpp
			   src/tab/Tablet.cpp
			   src/tab/tablet_util.cpp
			   src/tab/tablet_props.cpp
			   src/graphics/TestWindow.cpp
			   src/graphics/D2DWindow.cpp
			   src/graphics/TabletWin.cpp
			   src/graphics/direct2d_helpers.cpp
			   src/util/random.cpp
			   src/graphics/D3D10Window.cpp
			   src/graphics/imgui/ImGuiHandler.cpp
			   src/graphics/D2DDrawer.cpp
			   src/util/colorspaces.cpp
			   src/graphics/D2DBitmapAdapter.cpp
			   src/graphics/TabletDrawer.cpp
			   )

target_include_directories(npp PRIVATE
						   external/imgui
						   external/magic_enum
						   )

target_link_libraries(npp PRIVATE imgui_win32)
