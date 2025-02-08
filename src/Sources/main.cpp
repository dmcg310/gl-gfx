#include "gl_gfx.hpp"
#include "window.hpp"
#include "renderer.hpp"

#include <GLFW/glfw3.h>

// TODO: read from config file?
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int w,
                               int h);

int main(void) {
  Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "gl-gfx");

  gladLoadGL();
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);

  Renderer renderer;

  while (!window.should_close()) {
    window.handle_input();

    renderer.render();

    window.swap_buffers();
    window.poll_events();
  }

  return 0;
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int w,
                               int h) {
  glViewport(0, 0, w, h);
}
