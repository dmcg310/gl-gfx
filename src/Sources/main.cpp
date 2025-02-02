#include "gl_gfx.hpp"
#include "window.hpp"

int main(int argc, char *argv[]) {
  Window window(1600, 900, "gl-gfx");

  gladLoadGL();

  while (!window.should_close()) {
    window.poll_events();

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    window.swap_buffers();
  }

  return 0;
}
