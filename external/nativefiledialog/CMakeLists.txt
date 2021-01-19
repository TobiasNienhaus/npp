cmake_minimum_required(VERSION 3.17)
project(npp)

set(CMAKE_CXX_STANDARD 20)

add_library(nativefiledialog
			include/nfd.h
			nfd_common.c
			nfd_common.h
			nfd_win.cpp
			simple_exec.h
			common.h
			)
