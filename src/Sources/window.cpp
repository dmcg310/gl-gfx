#include "window.hpp"
#include "error_handler.hpp"

Window::Window(int w, int h, const char *t) : m_width(w), m_height(h) {
  if (!glfwInit()) {
    ErrorHandler::throwError("Failed to initialize GLFW", __FILE__, __func__,
                             __LINE__);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  m_handle = glfwCreateWindow(w, h, t, nullptr, nullptr);
  if (m_handle == nullptr) {
    ErrorHandler::throwError("Failed to create GLFW window", __FILE__, __func__,
                             __LINE__);
  }

  glfwMakeContextCurrent(m_handle);
}

Window::~Window() {
  if (m_handle) {
    glfwDestroyWindow(m_handle);
  }

  glfwTerminate();
}

bool Window::should_close() const { return glfwWindowShouldClose(m_handle); }

void Window::set_should_close(bool value) {
  glfwSetWindowShouldClose(m_handle, value);
}

void Window::swap_buffers() { glfwSwapBuffers(m_handle); }

void Window::poll_events() { glfwPollEvents(); }

void Window::handle_input() {
  if (glfwGetKey(m_handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    set_should_close(true);
  }
}
