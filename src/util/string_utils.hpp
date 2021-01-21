//
// Created by Tobias on 1/21/2021.
//

#ifndef NPP_STRING_UTILS_HPP
#define NPP_STRING_UTILS_HPP

#include <string>

namespace npp::str_util {

std::string wstr_to_str(const std::wstring &wstr);
std::wstring str_to_wstr(const std::string &str);

}

#endif // NPP_STRING_UTILS_HPP
