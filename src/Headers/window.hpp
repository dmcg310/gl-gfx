#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Window {
public:
  Window(int w, int h, const char *t);
  ~Window();

  GLFWwindow *get() const { return m_handle; }
  int get_width() const { return m_width; }
  int get_height() const { return m_height; }

  bool should_close() const;

  void set_should_close(bool value);

  void swap_buffers();
  void poll_events();

  void handle_input();

private:
  GLFWwindow *m_handle;
  int m_width;
  int m_height;
};

#endif // WINDOW_HPP
