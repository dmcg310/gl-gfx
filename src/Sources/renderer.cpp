#include "renderer.hpp"
#include "error_handler.hpp"
#include "shader_manager.hpp"

Renderer::Renderer() { init(); }

Renderer::~Renderer() {
  glDeleteBuffers(1, &m_VBO);
  glDeleteVertexArrays(1, &m_VAO);
}

void Renderer::init() {
  ShaderManager shaderManager;
  m_shaderProgram = shaderManager.create_program(
      "../src/Shaders/triangle.vert", "../src/Shaders/triangle.frag");
  if (m_shaderProgram == 0) {
    ErrorHandler::throwError("Failed to create shader program", __FILE__,
                             __func__, __LINE__);
  }

  glGenBuffers(1, &m_VBO);
  glGenVertexArrays(1, &m_VAO);

  std::vector<Vertex> vertices = {Vertex(-0.5f, -0.5f, 0.0f),
                                  Vertex(0.5f, -0.5f, 0.0f),
                                  Vertex(0.0f, 0.5f, 0.0f)};
  m_vertices = vertices;

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                       (void *)offsetof(Vertex, x));
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);

  // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                       (void *)offsetof(Vertex, u));
  // glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Renderer::render() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(m_shaderProgram);

  glBindVertexArray(m_VAO);
  glDrawArrays(GL_TRIANGLES, 6, m_vertices.size());
}
