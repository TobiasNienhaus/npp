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

void glfw_error(int error, const char *description);

void imgui_init(GLFWwindow *win);
void imgui_newframe();
void imgui_render();

static WNDPROC g_currentProc;

const char *get_type_name(POINTER_INPUT_TYPE type) {
	switch (type) {
	case PT_POINTER:
		return "POINTER";
	case PT_TOUCH:
		return "TOUCH";
	case PT_PEN:
		return "PEN";
	case PT_MOUSE:
		return "MOUSE";
	case PT_TOUCHPAD:
		return "TOUCHPAD";
	default:
		return "SOMETHING";
	}
}

bool down = false;
using pointerid_t = unsigned short;
pointerid_t pointer;

static LRESULT CALLBACK wndProc(HWND hwnd, UINT umsg, WPARAM wparam,
								LPARAM lparam) {
	switch (umsg) {
	case WM_POINTERENTER: {
		auto id = GET_POINTERID_WPARAM(wparam);
		POINTER_INPUT_TYPE type;
		if (!GetPointerType(id, &type)) {
			std::cout << "SOMETHING FAILED!" << GetLastError() << '\n';
		}
		std::cout << get_type_name(type) << " ENTERED!\n";
	} break;
	case WM_POINTERLEAVE: {
		auto id = GET_POINTERID_WPARAM(wparam);
		POINTER_INPUT_TYPE type;
		if (!GetPointerType(id, &type)) {
			std::cout << "SOMETHING FAILED!" << GetLastError() << '\n';
		}
		std::cout << get_type_name(type) << " LEFT!\n";
	} break;
	case WM_POINTERDOWN: {
		pointerid_t id = GET_POINTERID_WPARAM(wparam);
		POINTER_INPUT_TYPE type;
		if (!GetPointerType(id, &type)) {
			std::cout << "SOMETHING FAILED!" << GetLastError() << '\n';
		}
		if (type == PT_PEN) {
			down = true;
			pointer = id;
			POINTER_PEN_INFO info;
			GetPointerPenInfo(id, &info);
			std::cout << "X: " << info.pointerInfo.ptPixelLocation.x
					  << " | Y: " << info.pointerInfo.ptPixelLocation.y << '\n';
		}
	} break;
	case WM_POINTERUP: {
		pointerid_t id = GET_POINTERID_WPARAM(wparam);
		POINTER_INPUT_TYPE type;
		if (!GetPointerType(id, &type)) {
			std::cout << "SOMETHING FAILED!" << GetLastError() << '\n';
		}
		if (type == PT_PEN) {
			down = false;
			pointer = id;
			POINTER_PEN_INFO info;
			GetPointerPenInfo(id, &info);
			std::cout << "X: " << info.pointerInfo.ptPixelLocation.x
					  << " | Y: " << info.pointerInfo.ptPixelLocation.y << '\n';
		}
	} break;
	default:
		return CallWindowProc(g_currentProc, hwnd, umsg, wparam, lparam);
	}
	return 0;
}

void try_print_info();

int main() {
	if (!glfwInit()) {
		std::cerr << "Init crashed!\n";
		glfwTerminate();
		return 1;
	}

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
	glfwSwapInterval(0);

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

		try_print_info();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void try_print_info() {
	if(down) {
		POINTER_INPUT_TYPE type;
		if (!GetPointerType(pointer, &type)) {
			std::cout << "SOMETHING FAILED 1!" << GetLastError() << '\n';
			return;
		}
		if (type == PT_PEN) {
			POINTER_PEN_INFO info[512];
			UINT32 count = 512;
			std::cout << "POINTER " << pointer << '\n';
			if(!GetPointerPenInfoHistory(pointer, &count, info)) {
				std::cout << "SOMETHING FAILED 2!" << GetLastError() << '\n';
				std::cout << "TYPE MISMATCH IS CODE " << ERROR_DATATYPE_MISMATCH << '\n';
			} else {
				std::cout << "COUNT: " << count << '\n';
				for (int i = 0; i < count; ++i) {
					std::cout
						<< "POS [" << i << "] "
						<< "X: " << info[i].pointerInfo.ptPixelLocation.x
						<< " | Y: " << info[i].pointerInfo.ptPixelLocation.y
						<< " | PRESS: " << info[i].pressure
						<< '\n';
				}
			}

		}
	}
}

void imgui_init(GLFWwindow *win) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplOpenGL3_Init("#version 460 core");
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
