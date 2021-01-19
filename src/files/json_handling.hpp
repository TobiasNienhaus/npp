//
// Created by Tobias on 1/19/2021.
//

#ifndef NPP_JSON_HANDLING_HPP
#define NPP_JSON_HANDLING_HPP

#include <nlohmann/json_fwd.hpp>

namespace npp::tablet_types {

struct PointData;
class Line;

void to_json(nlohmann::json &j, const PointData &pd);
void from_json(const nlohmann::json &j, PointData &pd);

void to_json(nlohmann::json &j, const Line &pd);
void from_json(const nlohmann::json &j, Line &pd);

} // namespace npp::tablet_types

#endif // NPP_JSON_HANDLING_HPP
