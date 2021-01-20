//
// Created by Tobias on 1/19/2021.
//

#ifndef NPP_FILEHANDLING_HPP
#define NPP_FILEHANDLING_HPP

//#include "../util/win_headers.hpp"

#include <optional>
#include <string>
#include <vector>

namespace npp::tablet_types {

struct PointData;
class Line;

} // namespace npp::tablet_types

namespace npp::file {

using data_t = std::vector<npp::tablet_types::Line>;

enum class OpenMode {
	OPEN,
	SAVE
};

std::optional<std::string> get_filename(OpenMode mode);
bool save_data(const std::string &filename, const data_t &data);
data_t load_data(const std::string &filename);

}

#endif // NPP_FILEHANDLING_HPP
