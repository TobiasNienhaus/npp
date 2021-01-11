#define UNICODE
#define _UNICODE

#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winuser.h>

#include "graphics/TabletWin.hpp"

#include "tab/tablet_handling.hpp"
#include "tab/tablet_props.hpp"

void print_all_device_properties();

void weird_console_hack() {
	AllocConsole();
	FILE *fp;

	freopen_s(&fp, "CONOUT$", "w", stdout);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPWSTR lpCmdLine, int nCmdShow) {
	weird_console_hack();
	TabletWin window;

	if(!window.create(L"NPP", WS_OVERLAPPEDWINDOW)) {
		return 1;
	}

	window.show(nCmdShow);

	MSG msg{};
	while(GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

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
		std::cerr << "SOMETHING FAILED! (print_all_device_properties)" << GetLastError()
				  << '\n';
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
