#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <vector>

#include "types.hpp"

class Renderer {
public:
  Renderer();
  ~Renderer();

  void init();
  void render();

private:
  std::vector<Vertex> m_vertices;
  GLuint m_VBO;
  GLuint m_VAO;
  GLuint m_shaderProgram;
};

#endif // RENDERER_HPP
