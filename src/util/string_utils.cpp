//
// Created by Tobias on 1/21/2021.
//

#include "string_utils.hpp"

#include "win_headers.hpp"

namespace npp::str_util {

std::string wstr_to_str(const std::wstring &wstr) {
	int cNum = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0,
								   nullptr, nullptr);
	auto *cStr = new char[cNum];
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, cStr, cNum, nullptr,
						nullptr);

	std::string ret{cStr};

	delete[] cStr;
	return ret;
}

std::wstring str_to_wstr(const std::string &str) {
	int cNum = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	auto *wChars = new wchar_t[cNum];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wChars, cNum);

	std::wstring ret{wChars};

	delete[] wChars;
	return ret;
}

} // namespace npp::str_util
