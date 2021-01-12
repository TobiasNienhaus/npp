//
// Created by Tobias on 1/11/2021.
//

#ifndef NPP_DIRECT2D_HELPERS_HPP
#define NPP_DIRECT2D_HELPERS_HPP

#include <concepts>

#include "../util/win_headers.hpp"

#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

namespace npp::graphics {

template<typename T>
void com_safe_release(T **comObj) requires std::is_base_of_v<IUnknown, T> {
	if (*comObj) {
		(*comObj)->Release();
		*comObj = nullptr;
	}
}

}

#endif // NPP_DIRECT2D_HELPERS_HPP
