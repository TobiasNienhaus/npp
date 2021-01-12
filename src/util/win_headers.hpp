//
// Created by Tobias on 1/12/2021.
//

#ifndef NPP_WIN_HEADERS_HPP
#define NPP_WIN_HEADERS_HPP

// Unicode support
#define UNICODE
#define _UNICODE
// **** Microsoft
#define NOMINMAX
// Don't need bloat
#define WIN32_LEAN_AND_MEAN
// The actual windows header
#include <Windows.h>

#include <atlbase.h>

#endif // NPP_WIN_HEADERS_HPP
