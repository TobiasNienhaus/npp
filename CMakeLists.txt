cmake_minimum_required(VERSION 3.17)
project(npp)

set(CMAKE_CXX_STANDARD 20)

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
			   )

target_include_directories(npp PRIVATE
						   external/magic_enum
						   )
