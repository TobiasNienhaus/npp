#include <iostream>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>

#include "WinTablet.h"
#include "wacom_def.h"

void glfw_error(int error, const char *description);

void imgui_init(GLFWwindow *win);
void imgui_newframe();
void imgui_render();

static WinTablet g_tablet{};
static WinTablet::Status g_currStatus{};
static WNDPROC g_currentProc;

static LRESULT CALLBACK wndProc(HWND hwnd, UINT umsg, WPARAM wparam,
								LPARAM lparam) {
//	std::cout << "CALLBACK MESSAGE\n";
	switch (umsg) {
	case WM_CREATE:
		std::cout << "CREATE\n";
		g_tablet.init(hwnd);
		break;
	case WM_DESTROY:
		std::cout << "DESTROY\n";
		g_tablet.cleanup();
//		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		std::cout << "ACTIVATE\n";
		g_tablet.on_wm_activate(hwnd, wparam, lparam);
		return 0;
	case WT_PACKET: {
		std::cout << "PACKET\n";
		const WinTablet::Status newStatus =
			g_tablet.on_wt_packet(hwnd, wparam, lparam);
		if (memcmp(&newStatus, &g_currStatus, sizeof(newStatus)) != 0) {
			g_currStatus = newStatus;
			InvalidateRect(hwnd, nullptr, TRUE);
		}
		return 0;
	}
	case WM_PAINT: {
		std::cout << "PAINT\n";
		PAINTSTRUCT ps;
		const HDC hdc = BeginPaint(hwnd, &ps);
		const auto &status = g_currStatus;
		{
			POINT pos = {status.x, status.y};
			ScreenToClient(hwnd, &pos);

			const double x = pos.x;
			const double y = pos.y;
			// TODO support different levels of pressure
			const double pressure =
				static_cast<double>(status.pressure) / 1023.0;
			const bool invert = false;
			// TODO what about multiple buttons?
			const bool sideButton = status.button;

			// TODO This is some drawing stuff, might not be needed
			int fnObject = NULL_BRUSH;
			if (sideButton) { fnObject = GRAY_BRUSH; }

			const double pv = pressure * 128.0;
			const int cx = static_cast<int>(x);
			const int cy = static_cast<int>(y);
			const int left = static_cast<int>(x - pv);
			const int right = static_cast<int>(x + pv);
			const int top = static_cast<int>(y - pv);
			const int bottom = static_cast<int>(y + pv);

			SelectObject(hdc, GetStockObject(fnObject));
			if (invert) {
				RoundRect(hdc, left, top, right, bottom, 1, 1);
			} else {
				Ellipse(hdc, left, top, right, bottom);
			}
			MoveToEx(hdc, cx, 0, nullptr);
			LineTo(hdc, cx, GetDeviceCaps(hdc, VERTRES));
			MoveToEx(hdc, 0, cy, nullptr);
			LineTo(hdc, GetDeviceCaps(hdc, HORZRES), cy);
		}
		EndPaint(hwnd, &ps);
	} break;
	default:
		return CallWindowProc(g_currentProc, hwnd, umsg, wparam, lparam);
	}
	return 0;
//	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

int main() {
	if (!g_tablet.app_init()) { return 1; }

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
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "GLAD could not be initialized!\n";
		glfwTerminate();
		return 1;
	}

	// TODO scope maybe not the best idea
	{
		g_currentProc = (WNDPROC)GetWindowLongPtr(glfwGetWin32Window(window), GWL_WNDPROC);
		SetWindowLongPtr(glfwGetWin32Window(window), GWL_WNDPROC, (long)wndProc);
	}

	imgui_init(window);

	MSG msg = {nullptr};
	bool open = true;
	while (open) {
		if (glfwWindowShouldClose(window)) {
			open = false;
			continue;
		}
		if (!GetMessage(&msg, nullptr, 0, 0)) {
			open = false;
			continue;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClear(GL_COLOR_BUFFER_BIT);
		imgui_newframe();
		imgui_render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	g_tablet.app_cleanup();
	return static_cast<int>(msg.wParam);
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
