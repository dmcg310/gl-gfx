#include "window.hpp"
#include "error_handler.hpp"

Window::Window(int w, int h, const char *t) : width(w), height(h) {
  if (!glfwInit()) {
    ErrorHandler::throwError("Failed to initialize GLFW", __FILE__, __func__,
                             __LINE__);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  handle = glfwCreateWindow(w, h, t, nullptr, nullptr);
  if (handle == nullptr) {
    ErrorHandler::throwError("Failed to create GLFW window", __FILE__, __func__,
                             __LINE__);
  }

  glfwMakeContextCurrent(handle);
}

Window::~Window() {
  if (handle) {
    glfwDestroyWindow(handle);
  }

  glfwTerminate();
}

bool Window::should_close() const { return glfwWindowShouldClose(handle); }

void Window::set_should_close(int value) {
  glfwSetWindowShouldClose(handle, value);
}

void Window::swap_buffers() { glfwSwapBuffers(handle); }

void Window::poll_events() { glfwPollEvents(); }
