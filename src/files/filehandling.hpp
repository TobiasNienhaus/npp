//
// Created by Tobias on 1/19/2021.
//

#ifndef NPP_FILEHANDLING_HPP
#define NPP_FILEHANDLING_HPP

#include <string>

#include "../util/win_headers.hpp"

namespace npp::tablet_types {

struct PointData;
class Line;

} // namespace npp::tablet_types

namespace npp::file {

HRESULT file_open_dialogue(HWND owner = nullptr);

}

#endif // NPP_FILEHANDLING_HPP
