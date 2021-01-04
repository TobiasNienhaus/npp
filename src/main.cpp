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

constexpr const char *g_vertSource = R"(
#version 460

layout (location = 0) in vec2 vert;
layout (location = 1) in float press;

uniform ivec2 screenPos;
uniform ivec2 screenSize;

out float pressure;

void main() {
	gl_Position = vec4((((vert - screenPos) / screenSize) * 2 - 1) * vec2(1, -1), 0, 1);
	pressure = press;
}
)";

constexpr const char *g_fragSource = R"(
#version 460

in float pressure;

out vec4 col;

void main() {
	gl_FragColor = vec4(clamp(pressure, 0, 1));
}
)";

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

void GLAPIENTRY opengl_message_callback(
	[[maybe_unused]] GLenum source, GLenum type, [[maybe_unused]] GLuint id,
	GLenum severity, [[maybe_unused]] GLsizei length, const GLchar *message,
	[[maybe_unused]] const void *userParam) {
	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		std::cerr << "GL ERROR: type " << type << " message: " << message
				  << '\n';
	} else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
		std::cerr << "GL WARNING: type " << type << " message: " << message
				  << '\n';
	}
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
	glfwSwapInterval(0);

	// TODO scope maybe not the best idea
	{
		g_currentProc =
			(WNDPROC)GetWindowLongPtr(glfwGetWin32Window(window), GWL_WNDPROC);
		SetWindowLongPtr(glfwGetWin32Window(window), GWL_WNDPROC,
						 (long)wndProc);
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "GLAD could not be initialized!\n";
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(opengl_message_callback, nullptr);

	GLuint shader = glCreateProgram();

	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &g_vertSource, nullptr);
	glCompileShader(vert);

	GLint compileStatus = 0;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		std::cout << "Frag Shader compilation failed:" << std::endl;

		GLint logLength = 0;
		glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			auto *errorLog = new GLchar[logLength];
			glGetShaderInfoLog(vert, logLength, nullptr, errorLog);
			std::cout << errorLog << std::endl;
			delete[] errorLog;
		} else {
			std::cout << "No info available." << std::endl;
		}
		return 1;
	}

	// FIXME should check errors
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &g_fragSource, nullptr);
	glCompileShader(frag);

	compileStatus = 0;
	glGetShaderiv(frag, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		std::cout << "Vert Shader compilation failed:" << std::endl;

		GLint logLength = 0;
		glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			auto *errorLog = new GLchar[logLength];
			glGetShaderInfoLog(frag, logLength, nullptr, errorLog);
			std::cout << errorLog << std::endl;
			delete[] errorLog;
		} else {
			std::cout << "No info available." << std::endl;
		}
		return 1;
	}

	glAttachShader(shader, vert);
	glAttachShader(shader, frag);
	glLinkProgram(shader);

	glDeleteShader(vert);
	glDeleteShader(frag);
	glUseProgram(shader);

	std::vector<npp::Tablet::PointData> points{};

	imgui_init(window);

	glPointSize(10.0);

	std::cout << "OFFSET " << offsetof(npp::Tablet::PointData, valid) << ", "
			  << offsetof(npp::Tablet::PointData, x) << ", "
			  << offsetof(npp::Tablet::PointData, y) << ", "
			  << offsetof(npp::Tablet::PointData, pressure) << '\n';

	for (int i = 0; i < 10; ++i) {
		points.emplace_back(true, i / 10.f, i / 10.f, 0.5f);
	}

	glUniform2i(glGetUniformLocation(shader, "screenSize"), 1280, 720);

	MSG msg = {nullptr};
	bool open = true;
	while (open) {
		if (glfwWindowShouldClose(window)) {
			open = false;
			continue;
		}
//		// Are messages coming fast enough? Maybe put on another thread
//		if (!GetMessage(&msg, nullptr, 0, 0)) {
//			open = false;
//			continue;
//		} else {
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
		int xpos, ypos;
		glfwGetWindowPos(window, &xpos, &ypos);
		glUseProgram(shader);
		glUniform2i(glGetUniformLocation(shader, "screenPos"), xpos, ypos);

		//	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		npp::tablet().update();

		auto newPoints = npp::tablet().get_all();
		std::copy(newPoints.begin(), newPoints.end(),
				  std::back_inserter(points));
//		if (!newPoints.empty()) {
//			for (auto &p : points) {
//				std::cout << p.x << ", " << p.y << ", " << p.pressure << '\n';
//			}
//		}

		if (!points.empty()) {
			GLuint vao, vbo;

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat) * points.size(),
						 points.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(
				0, 2, GL_FLOAT, GL_FALSE, sizeof(npp::Tablet::PointData),
				(void *)(offsetof(npp::Tablet::PointData, x)));
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
				1, 1, GL_FLOAT, GL_FALSE, sizeof(npp::Tablet::PointData),
				(void *)(offsetof(npp::Tablet::PointData, pressure)));
			glEnableVertexAttribArray(1);

			glDrawArrays(GL_POINTS, 0, points.size());

			glDeleteBuffers(1, &vbo);
			glDeleteVertexArrays(1, &vao);
		}
		imgui_newframe();

		ImGui::Begin("FPS");
		ImGui::Value("FPS: ", ImGui::GetIO().Framerate);
		ImGui::End();

		imgui_render();

		glfwSwapBuffers(window);
		glfwPollEvents();
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
		std::cerr << "SOMETHING FAILED! (print_props)" << GetLastError()
				  << '\n';
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

void test_something() {
	constexpr UINT32 maxDevices = 32;
	UINT32 deviceCount = maxDevices;
	POINTER_DEVICE_INFO devices[maxDevices];
	if (!GetPointerDevices(&deviceCount, devices)) {
		std::cerr << "SOMETHING FAILED! (test_something)" << GetLastError()
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
		print_props(devices[i].device);
	}
}
