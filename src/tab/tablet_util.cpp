//
// Created by Tobias on 12/14/2020.
//

#include "tablet_util.hpp"

#include <iostream>

namespace npp::util {

PointerType id_to_type(Tablet::pointerid_t id) {
	POINTER_INPUT_TYPE type;
	if (!GetPointerType(id, &type)) {
		std::cerr << "Could not get type from pointer id!" << GetLastError()
				  << " (" << __FILE__ << ": " << __LINE__ << ")\n";
		return PointerType::INVALID;
	}
	return win32_ptype_to_lib(type);
}

} // namespace npp::util
