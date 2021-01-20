//
// Created by Tobias on 1/19/2021.
//

#include "filehandling.hpp"

#include "json_handling.hpp"
#include "../tab/Tablet.hpp"
#include <nlohmann/json.hpp>

#include <Shlobj.h>
#include <iostream>

#include <nfd.h>

#include <fstream>

static std::optional<std::string> load_file_as_string(const std::string &path) {
	std::ifstream file{path};
	if(file.is_open()) {
		std::string contents;
		file.seekg(0, std::ios::end);
		contents.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&contents[0], contents.size());
		file.close();
		return(contents);
	} else {
		return {};
	}
}

namespace npp::file {

std::optional<std::string> get_filename(OpenMode mode) {
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

	nfdresult_t result = NFD_ERROR;
	switch(mode) {
	case OpenMode::OPEN:
		result = NFD_OpenDialog("npp;bnpp", cStr, &outPath);
		break;
	case OpenMode::SAVE:
		result = NFD_SaveDialog("npp;bnpp", cStr, &outPath);
		break;
	}

	delete[] cStr;
	CoTaskMemFree(documentsPath);

	if (result == NFD_OKAY) {
		std::string ret{outPath};

//		int charNum = MultiByteToWideChar(CP_UTF8, 0, outPath, -1, nullptr, 0);
//		auto *wstr = new wchar_t[charNum];
//		MultiByteToWideChar(CP_UTF8, 0, outPath, -1, wstr, charNum);
//		std::string str{outPath};
//		delete[] wstr;
		delete[] outPath;
		return ret;
	} else if (result == NFD_CANCEL) {
		return std::nullopt;
	} else {
		std::cout << "Error: " << NFD_GetError() << '\n';
		return std::nullopt;
	}
}

bool save_data(const std::string &filename,
			   const data_t &data) {
	// TODO handle different filetypes
	using nlohmann::json;
	std::ofstream file{filename, std::ios::trunc};
	if(file.is_open()) {
		json js = json::object();
		js["data"] = data;
		file << js.dump();
		file.close();
		return true;
	} else {
		return false;
	}
}

data_t load_data(const std::string& filename) {
	using nlohmann::json;
	auto data{load_file_as_string(filename)};
	if(data.has_value()) {
		auto j = json::parse(data.value());
		data_t d;
		j.at("data").get_to(d);
		return d;
	} else {
		return data_t();
	}
}

} // namespace npp::file
