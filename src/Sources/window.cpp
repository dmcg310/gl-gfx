#include "window.hpp"

#include <iostream>

Window::Window(int w, int h, const char *t) : width(w), height(h) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  handle = glfwCreateWindow(w, h, t, nullptr, nullptr);
  if (handle == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
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
