//
// Created by Tobias on 1/19/2021.
//

#include "json_handling.hpp"

#include <nlohmann/json.hpp>

#include "../tab/Tablet.hpp"

namespace npp::tablet_types {

using nlohmann::json;

void to_json(json &j, const PointData &pd) {
	j = nlohmann::json{{"valid", pd.valid},
					   {"x", pd.x},
					   {"y", pd.y},
					   {"pressure", pd.pressure}};
}

void from_json(const json &j, PointData &pd) {
	j.at("valid").get_to(pd.valid);
	j.at("x").get_to(pd.x);
	j.at("y").get_to(pd.y);
	j.at("pressure").get_to(pd.pressure);
}

void to_json(json &j, const Line &line) {
	j = json{{"points", line.points()}};
}

void from_json(const json &j, Line &line) {
	j.at("points").get_to(line.points());
}

} // namespace npp::tablet_types
