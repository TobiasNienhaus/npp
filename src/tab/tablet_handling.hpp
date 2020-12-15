//
// Created by Tobias on 12/14/2020.
//

#ifndef NPP_TABLET_HANDLING_HPP
#define NPP_TABLET_HANDLING_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Tablet.hpp"

namespace npp {

Tablet &tablet();

LRESULT catch_tablet_msgs(WNDPROC next, HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

}

#endif // NPP_TABLET_HANDLING_HPP
