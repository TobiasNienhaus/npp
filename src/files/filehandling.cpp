//
// Created by Tobias on 1/19/2021.
//

#include "filehandling.hpp"

#include <Shlobj.h>
#include <iostream>

#include <nfd.h>

namespace npp::file {

std::optional<std::wstring> get_filename() {
	PWSTR documentsPath = nullptr;
	HRESULT hr =
		SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &documentsPath);
	if (FAILED(hr)) {
		CoTaskMemFree(documentsPath);
		return {};
	}

	int cNum = WideCharToMultiByte(CP_UTF8, 0, documentsPath, -1, nullptr, 0,
								   nullptr, nullptr);
	auto *cStr = new char[cNum];
	WideCharToMultiByte(CP_UTF8, 0, documentsPath, -1, cStr, cNum, nullptr,
						nullptr);

	std::cout << "Default path: " << cStr << '\n';

	nfdchar_t *outPath{nullptr};
	nfdresult_t result = NFD_OpenDialog("npp;bnpp", cStr, &outPath);

	delete[] cStr;
	CoTaskMemFree(documentsPath);

	if (result == NFD_OKAY) {
		std::string ret{outPath};

		int charNum = MultiByteToWideChar(CP_UTF8, 0, outPath, -1, nullptr, 0);
		auto *wstr = new wchar_t[charNum];
		MultiByteToWideChar(CP_UTF8, 0, outPath, -1, wstr, charNum);
		std::wstring str{wstr};
		delete[] wstr;
		delete[] outPath;
		return str;
	} else if (result == NFD_CANCEL) {
		return std::nullopt;
	} else {
		std::cout << "Error: " << NFD_GetError() << '\n';
		return std::nullopt;
	}
}

} // namespace npp::file
