#define UNICODE
#define _UNICODE

#include <iostream>
#include <nlohmann/json.hpp>

#include "util/win_headers.hpp"

#include "graphics/D3D10Window.hpp"

#include "files/json_handling.hpp"
#include "tab/Tablet.hpp"
#include "util/random.h"

#include "files/filehandling.hpp"

#include "tab/tablet_props.hpp"

void print_all_device_properties();

void weird_console_hack() {
	AllocConsole();
	FILE *fp;

	freopen_s(&fp, "CONOUT$", "w", stdout);
}

void json_test();
void file_test();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPWSTR lpCmdLine, int nCmdShow) {
	auto hr = CoInitializeEx(nullptr,
							 COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr)) {
		PostQuitMessage(-1);
		return -1;
	}
	npp::file::prepare_folders();
#ifdef DEBUG
	weird_console_hack();
#endif
	D3D10Window window;

	if (!window.create(L"NPP", WS_OVERLAPPEDWINDOW)) { return 1; }

	window.show(nCmdShow);

	MSG msg{};
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CoUninitialize();
	return 0;
}

void print_device_properties(HANDLE deviceHandle) {
	constexpr UINT32 maxProperties = 32;
	UINT32 propertyCount = maxProperties;
	POINTER_DEVICE_PROPERTY properties[maxProperties];
	if (!GetPointerDeviceProperties(deviceHandle, &propertyCount, properties)) {
		std::cerr << "SOMETHING FAILED! (print_device_properties)"
				  << GetLastError() << '\n';
		return;
	}
	for (int i = 0; i < propertyCount; ++i) {
		auto &prop = properties[i];
		std::cout << "logical min: " << prop.logicalMin << '\n'
				  << "logical max: " << prop.logicalMax << '\n'
				  << "physical min: " << prop.physicalMin << '\n'
				  << "physical max: " << prop.physicalMax << '\n'
				  << "unit: " << prop.unit << '\n'
				  << "unit exponent: " << prop.unitExponent << '\n'
				  << "usage page id: " << prop.usagePageId << '\n'
				  << "usage id: " << prop.usageId << '\n'
				  << "usage name: "
				  << npp::props::usage_as_string(prop.usagePageId, prop.usageId)
				  << '\n';
	}
}

void print_all_device_properties() {
	constexpr UINT32 maxDevices = 32;
	UINT32 deviceCount = maxDevices;
	POINTER_DEVICE_INFO devices[maxDevices];
	if (!GetPointerDevices(&deviceCount, devices)) {
		std::cerr << "SOMETHING FAILED! (print_all_device_properties)"
				  << GetLastError() << '\n';
		return;
	}
	for (int i = 0; i < deviceCount; ++i) {
		char str[520];
		size_t num;
		auto err = wcstombs_s(&num, str, 520, devices[i].productString, 519);
		if (!err) {
			std::cout << str << '\n';
		} else
			std::cerr << "String convert error!\n";
		print_device_properties(devices[i].device);
	}
}

void json_test() {
	namespace tt = npp::tablet_types;
	using nlohmann::json;
	std::vector<tt::Line> lines;
	const unsigned int lineCount = npp::rnd::random(4u, 8u);
	for (int i = 0; i < lineCount; ++i) {
		auto &line = lines.emplace_back();
		const unsigned int pointCount = npp::rnd::random(5u, 15u);
		for (int j = 0; j < pointCount; ++j) {
			line.push({npp::rnd::random<bool>(), npp::rnd::random(0.f, 1000.f),
					   npp::rnd::random(0.f, 1500.f),
					   npp::rnd::random_01<float>()});
		}
	}

	json j{lines};
	std::cout << j.dump(4) << '\n';
}
