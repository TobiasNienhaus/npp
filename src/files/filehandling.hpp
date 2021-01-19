//
// Created by Tobias on 1/19/2021.
//

#ifndef NPP_FILEHANDLING_HPP
#define NPP_FILEHANDLING_HPP

#include <string>
#include <optional>

#include "../util/win_headers.hpp"

namespace npp::tablet_types {

struct PointData;
class Line;

} // namespace npp::tablet_types

namespace npp::file {

std::optional<std::wstring> get_filename();

}

#endif // NPP_FILEHANDLING_HPP
