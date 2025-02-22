#include "backend.h"
#include "input.h"

#include <enums.h>
#include <iostream>
#include <string>

namespace Backend {
	GLFWwindow *m_window = NULL;
	GLFWmonitor *m_monitor;
	WindowedMode m_windowedMode = WindowedMode::WINDOWED;
	const GLFWvidmode *m_mode;
	bool m_forceCloseWindow = false;

	int m_windowedWidth = 0;
	int m_windowedHeight = 0;
	int m_fullscreenWidth = 0;
	int m_fullscreenHeight = 0;
	int m_currentWindowWidth = 0;
	int m_currentWindowHeight = 0;

	void framebuffer_resize_callback(GLFWwindow *window, int w, int h);

	void window_focus_callback(GLFWwindow *window, int focused);

	void Init() {
		const int width = 1600;
		const int height = 900;

		glfwInit();

		glfwSetErrorCallback([](int error, const char *description) {
			std::cout << "[GLFW Error](" << std::to_string(error) << "): " << description << "\n";
		});

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

		m_monitor = glfwGetPrimaryMonitor();
		m_mode = glfwGetVideoMode(m_monitor);

		glfwWindowHint(GLFW_RED_BITS, m_mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, m_mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, m_mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, m_mode->refreshRate);

		m_fullscreenWidth = m_mode->width;
		m_fullscreenHeight = m_mode->height;

		m_windowedWidth = width;
		m_windowedHeight = height;

		CreateGLFWWindow(WindowedMode::WINDOWED);
		if (m_window == NULL) {
			ErrorHandler::ThrowError("Failed to create GLFW window", __FILE__, __func__, __LINE__);
		}

		glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);
		glfwSetWindowFocusCallback(m_window, window_focus_callback);

		glfwMakeContextCurrent(m_window);

		if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
			ErrorHandler::ThrowError("Failed to initialise GLAD", __FILE__, __func__, __LINE__);
		}

		glEnable(GL_DEPTH_TEST);

		Input::Init();
	}

	void BeginFrame() {
		glfwPollEvents();
	}

	void Update() {
		Input::Update();
	}

	void EndFrame() {
		glfwSwapBuffers(m_window);
	}

	void CleanUp() {
		if (m_windowedMode == WindowedMode::FULLSCREEN) {
			ToggleFullscreen();
		}

		glfwTerminate();
	}

	GLFWwindow *GetWindowHandle() {
		return m_window;
	}

	void CreateGLFWWindow(const WindowedMode &windowedMode) {
		if (windowedMode == WindowedMode::WINDOWED) {
			m_currentWindowWidth = m_windowedWidth;
			m_currentWindowHeight = m_windowedHeight;
			m_window = glfwCreateWindow(m_windowedWidth, m_windowedHeight, "gl-gfx", NULL, NULL);
			glfwSetWindowPos(m_window, 0, 0);
		} else if (windowedMode == WindowedMode::FULLSCREEN) {
			m_currentWindowWidth = m_fullscreenWidth;
			m_currentWindowHeight = m_fullscreenHeight;
			m_window = glfwCreateWindow(m_fullscreenWidth, m_fullscreenHeight, "gl-gfx", m_monitor, NULL);
		}

		m_windowedMode = windowedMode;
	}

	void SetWindowedMode(const WindowedMode &windowedMode) {
		if (windowedMode == WindowedMode::WINDOWED) {
			m_currentWindowWidth = m_windowedWidth;
			m_currentWindowHeight = m_windowedHeight;
			glfwSetWindowMonitor(m_window, nullptr, 0, 0, m_windowedWidth, m_windowedHeight, m_mode->refreshRate);
			glfwSetWindowPos(m_window, 0, 0);
		} else if (windowedMode == WindowedMode::FULLSCREEN) {
			m_currentWindowWidth = m_fullscreenWidth;
			m_currentWindowHeight = m_fullscreenHeight;
			glfwSetWindowMonitor(m_window, nullptr, 0, 0, m_fullscreenWidth - 1, m_fullscreenHeight - 1,
			                     m_mode->refreshRate);
			glfwSetWindowPos(m_window, 0, 0);
		}

		m_windowedMode = windowedMode;
	}

	void ToggleFullscreen() {
		if (m_windowedMode == WindowedMode::WINDOWED) {
			SetWindowedMode(WindowedMode::FULLSCREEN);
		} else {
			SetWindowedMode(WindowedMode::WINDOWED);
		}
	}

	void ForceCloseWindow() {
		m_forceCloseWindow = true;
	}

	bool WindowIsOpen() {
		return !(glfwWindowShouldClose(m_window) || m_forceCloseWindow);
	}

	bool WindowIsMinimized() {
		int w = 0;
		int h = 0;

		glfwGetFramebufferSize(m_window, &w, &h);

		return (w == 0 || h == 0);
	}

	float GetWindowWidth() {
		return m_currentWindowWidth;
	}

	float GetWindowHeight() {
		return m_currentWindowHeight;
	}

	float GetWindowTime() {
		return glfwGetTime();
	}

	void framebuffer_resize_callback(GLFWwindow */*window*/, const int w, const int h) {
		glViewport(0, 0, w, h);
		m_currentWindowWidth = w;
		m_currentWindowHeight = h;
	}

	void window_focus_callback(GLFWwindow * /*window*/, int /*focused*/) {
	}
}
