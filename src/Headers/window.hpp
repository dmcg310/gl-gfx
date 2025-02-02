#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Window {
private:
  GLFWwindow *handle;
  int width;
  int height;

public:
  Window(int w, int h, const char *t);
  ~Window();

  GLFWwindow *get() const { return handle; }
  int get_width() const { return width; }
  int get_height() const { return height; }

  bool should_close() const;

  void set_should_close(int value);

  void swap_buffers();
  void poll_events();
};

#endif // WINDOW_HPP
