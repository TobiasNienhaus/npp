#include <iostream>

#include <glad/glad.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>

#include "tab/tablet_handling.hpp"
#include "tab/tablet_props.hpp"

void glfw_error(int error, const char *description);

void imgui_init(GLFWwindow *win);
void imgui_newframe();
void imgui_render();

void test_something();

static WNDPROC g_currentProc;

// SOURCES:
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getpointerpeninfohistory
// https://docs.microsoft.com/en-us/windows/win32/api/_inputmsg/
// https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-

// MAYBE CHECK OUT DIRECT2D
// https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart

static LRESULT CALLBACK wndProc(HWND hwnd, UINT umsg, WPARAM wparam,
								LPARAM lparam) {
	// TODO maybe suboptimal, maybe store wndproc in tablet_handling somewhere
	return npp::catch_tablet_msgs(g_currentProc, hwnd, umsg, wparam, lparam);
}

int main() {
	if (!glfwInit()) {
		std::cerr << "Init crashed!\n";
		glfwTerminate();
		return 1;
	}

	test_something();

	glfwSetErrorCallback(glfw_error);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
	if (!window) {
		std::cerr << "Window could not be initialized!\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "GLAD could not be initialized!\n";
		glfwTerminate();
		return 1;
	}

	// TODO scope maybe not the best idea
	{
		g_currentProc =
			(WNDPROC)GetWindowLongPtr(glfwGetWin32Window(window), GWL_WNDPROC);
		SetWindowLongPtr(glfwGetWin32Window(window), GWL_WNDPROC,
						 (long)wndProc);
	}

	imgui_init(window);

	MSG msg = {nullptr};
	bool open = true;
	while (open) {
		if (glfwWindowShouldClose(window)) {
			open = false;
			continue;
		}
		// Are messages coming fast enough? Maybe put on another thread
		if (!GetMessage(&msg, nullptr, 0, 0)) {
			open = false;
			continue;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		imgui_newframe();
		imgui_render();

		glfwSwapBuffers(window);
		glfwPollEvents();
		npp::tablet().update();
	}

	return 0;
}

void imgui_init(GLFWwindow *win) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplOpenGL3_Init("#version 460 core");
	// TODO call callbacks manually to also enable tablet callbacks
	ImGui_ImplGlfw_InitForOpenGL(win, true);
}

void imgui_newframe() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void imgui_render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void glfw_error(int error, const char *description) {
	std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

void print_props(HANDLE handle) {
	constexpr UINT32 maxProperties = 32;
	UINT32 propertyCount = maxProperties;
	POINTER_DEVICE_PROPERTY properties[maxProperties];
	if (!GetPointerDeviceProperties(handle, &propertyCount, properties)) {
		std::cerr << "SOMETHING FAILED! (print_props)" << GetLastError() << '\n';
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
		<< "usage name: " << npp::props::usage_as_string(prop.usagePageId, prop.usageId) << '\n';
	}
}

void test_something() {
	constexpr UINT32 maxDevices = 32;
	UINT32 deviceCount = maxDevices;
	POINTER_DEVICE_INFO devices[maxDevices];
	if (!GetPointerDevices(&deviceCount, devices)) {
		std::cerr << "SOMETHING FAILED! (test_something)" << GetLastError() << '\n';
		return;
	}
	for (int i = 0; i < deviceCount; ++i) {
		char str[520];
		size_t num;
		auto err = wcstombs_s(&num, str, 520, devices[i].productString, 519);
		if (!err) { std::cout << str << '\n'; }
		else std::cerr << "String convert error!\n";
		print_props(devices[i].device);
	}
}
