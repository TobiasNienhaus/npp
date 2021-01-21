//
// Created by Tobias on 1/19/2021.
//

#include "filehandling.hpp"

#include <nlohmann/json.hpp>
#include "../tab/Tablet.hpp"

#include <Shlobj.h>
#include <iostream>

#include <filesystem>
#include <fstream>

#include <snappy.h>

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

static const std::string &get_documents_path();

static bool dir_exists(const std::string &path) {
	DWORD ftyp = GetFileAttributesA(path.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES) {
		return false; // something is wrong with your path!
	}

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
		return true; // this is a directory!
	}

	return false; // this is not a directory!
}

namespace npp::file {

static data_t load_from_binary(const std::string &path);
static data_t load_from_text(const std::string &path);

static bool save_to_text(const std::string &path, const data_t &data);
static bool save_to_binary(const std::string &path, const data_t &data);

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

const std::string &get_default_path() {
	static bool initialized{};
	static std::string str{};
	if (!initialized) {
		str = get_documents_path() + "/npp/";
	}
	return str;
}

void prepare_folders() {
	auto path{get_documents_path() + "/npp/"};
	if(!dir_exists(path)) {
		// Create folder
		std::filesystem::create_directories(path);
	}
}

static data_t load_from_binary(const std::string &path) {
	using nlohmann::json;
	std::ifstream f(path, std::ios::in | std::ios::binary);
	if (f.is_open()) {
		f.unsetf(std::ios::skipws);

		// get its size:
		std::streampos fileSize;

		f.seekg(0, std::ios::end);
		fileSize = f.tellg();
		f.seekg(0, std::ios::beg);

		std::string str;
		str.reserve(fileSize);

		str.insert(str.begin(), std::istream_iterator<char>(f),
				   std::istream_iterator<char>());

		std::string o;

		snappy::Uncompress(str.data(), str.size(), &o);

		auto j = json::from_bson(o);
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
		std::string out{d.begin(), d.end()};
		std::string compressed;
		compressed.reserve(out.size());

		snappy::Compress(out.data(), out.size(), &compressed);

		file.write(compressed.data(),
				   compressed.size() * sizeof(std::string::value_type));
		file.close();
		return true;
	} else {
		return false;
	}
}

} // namespace npp::file

static const std::string &get_documents_path() {
	static bool initialized{};
	static std::string str{};
	if (!initialized) {
		PWSTR documentsPath = nullptr;
		HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr,
										  &documentsPath);
		if (FAILED(hr)) {
			CoTaskMemFree(documentsPath);
			str = "C:/";
		} else {
			int cNum = WideCharToMultiByte(CP_UTF8, 0, documentsPath, -1,
										   nullptr, 0, nullptr, nullptr);
			auto *cStr = new char[cNum];
			WideCharToMultiByte(CP_UTF8, 0, documentsPath, -1, cStr, cNum,
								nullptr, nullptr);
			str.assign(cStr);
			delete[] cStr;
		}
	}
	return str;
}
