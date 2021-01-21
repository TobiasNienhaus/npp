cmake_minimum_required(VERSION 3.17)
project(npp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/imgui)
add_subdirectory(external/json)
add_subdirectory(external/nativefiledialog)
add_subdirectory(external/ImGuiFileDialog)
add_subdirectory(external/snappy)

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
			   src/globals.cpp
			   src/files/json_handling.cpp
			   src/files/filehandling.cpp
			   src/files/filedialog.cpp
			   src/util/string_utils.cpp
			   res/icon.rc
			   )

target_include_directories(npp PRIVATE
						   external/imgui
						   external/json/include
						   external/magic_enum
						   external/nativefiledialog/include
						   external/ImGuiFileDialog
						   external/snappy
						   )

target_link_libraries(npp PRIVATE imgui_win32 nlohmann_json nativefiledialog ImGuiFileDialog snappy)
