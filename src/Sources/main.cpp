#include "gl_gfx.hpp"
#include "window.hpp"

#include <GLFW/glfw3.h>

// TODO: read from config file?
#define WINDOW_WIDTH 1600
#define WINDOW_HEGIHT 900

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int w,
                               int h);

int main(void) {
  Window window(WINDOW_WIDTH, WINDOW_HEGIHT, "gl-gfx");

  gladLoadGL();
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEGIHT);
  glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);

  while (!window.should_close()) {
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    window.handle_input();
    window.swap_buffers();
    window.poll_events();
  }

  return 0;
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int w,
                               int h) {
  glViewport(0, 0, w, h);
}
