//
// Created by Tobias on 1/19/2021.
//

#include "filehandling.hpp"

#include <nlohmann/json.hpp>
#include "../tab/Tablet.hpp"
#include "json_handling.hpp"

#include <Shlobj.h>
#include <iostream>

#include <nfd.h>

#include <filesystem>
#include <fstream>

static std::optional<std::string> load_file_as_string(const std::string &path) {
	std::ifstream file{path};
	if (file.is_open()) {
		std::string contents;
		file.seekg(0, std::ios::end);
		contents.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&contents[0], contents.size());
		file.close();
		return (contents);
	} else {
		return {};
	}
}

namespace npp::file {

static data_t load_from_binary(const std::string &path);
static data_t load_from_text(const std::string &path);

static bool save_to_text(const std::string &path, const data_t &data);
static bool save_to_binary(const std::string &path, const data_t &data);

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
	switch (mode) {
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

		//		int charNum = MultiByteToWideChar(CP_UTF8, 0, outPath, -1,
		// nullptr, 0); 		auto *wstr = new wchar_t[charNum];
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

bool save_data(const std::string &filename, const data_t &data) {
	using nlohmann::json;
	std::wstring ext{std::filesystem::path{filename}.extension().c_str()};
	if (ext == L".npp") {
		std::cout << "Save to plain: " << filename << '\n';
		return save_to_text(filename, data);
	} else if (ext == L".bnpp") {
		std::cout << "Save to binary: " << filename << '\n';
		return save_to_binary(filename, data);
	} else {
		return false;
	}
}

data_t load_data(const std::string &filename) {
	using nlohmann::json;
	std::wstring ext{std::filesystem::path{filename}.extension().c_str()};
	if (ext == L".npp") {
		return load_from_text(filename);
	} else if (ext == L".bnpp") {
		return load_from_binary(filename);
	} else {
		return data_t{};
	}
}

static data_t load_from_binary(const std::string &path) {
	using nlohmann::json;
	std::basic_ifstream<char8_t> f(path, std::ios::in | std::ios::binary);
	if (f.is_open()) {
		f.unsetf(std::ios::skipws);

		// get its size:
		std::streampos fileSize;

		f.seekg(0, std::ios::end);
		fileSize = f.tellg();
		f.seekg(0, std::ios::beg);

		std::vector<char8_t> d;
		d.reserve(fileSize);

		d.insert(d.begin(), std::istreambuf_iterator<char8_t>(f),
				 std::istreambuf_iterator<char8_t>());

		auto j = json::from_bson(d);
		//		auto j = json::parse(data.value());
		data_t data;
		j.at("data").get_to(data);
		return data;
	} else {
		return npp::file::data_t();
	}
}

static data_t load_from_text(const std::string &path) {
	using nlohmann::json;
	auto data{load_file_as_string(path)};
	if (data.has_value()) {
		auto j = json::parse(data.value());
		data_t d;
		std::cout << "TEXT: \n" << j.at("data").dump(2) << '\n';
		j.at("data").get_to(d);
		return d;
	} else {
		return npp::file::data_t();
	}
}

static bool save_to_text(const std::string &path, const data_t &data) {
	// TODO handle different filetypes
	using nlohmann::json;
	std::ofstream file{path, std::ios::trunc};
	if (file.is_open()) {
		json js = json::object();
		js["data"] = data;
		file << js.dump();
		file.close();
		return true;
	} else {
		return false;
	}
}

static bool save_to_binary(const std::string &path, const data_t &data) {
	// TODO handle different filetypes
	using nlohmann::json;
	std::ofstream file{path,
					   std::ios::trunc | std::ios::out | std::ios::binary};
	if (file.is_open()) {
		json js = json::object();
		js["data"] = data;
		auto d{json::to_bson(js)};
		file.write((char *)d.data(), d.size() * sizeof(char8_t));
		file.close();
		return true;
	} else {
		return false;
	}
}

} // namespace npp::file
