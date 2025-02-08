#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <glad/glad.h>
#include <string>
#include <unordered_map>

class ShaderManager {
public:
  enum ShaderType { VERTEX, FRAGMENT };

  ShaderManager() {};
  ~ShaderManager() {
    for (auto &pair : m_shaders) {
      glDeleteShader(pair.second);
    }

    for (auto &pair : m_programs) {
      glDeleteProgram(pair.second);
    }
  }

  GLuint get_shader(ShaderType type);
  GLuint create_program(const std::string &vPath, const std::string &fPath);

  GLenum shader_type_to_gl(ShaderType type) {
    return type == VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
  }
  const std::string shader_type_to_string(ShaderType type) {
    return type == VERTEX ? "VERTEX" : "FRAGMENT";
  }

private:
  std::unordered_map<ShaderType, unsigned int> m_shaders;
  std::unordered_map<std::string, unsigned int> m_programs;

  GLuint load_shader(ShaderType type, const std::string &path);

  void check_compile_errors(unsigned int shader, const std::string &type);
};

#endif // SHADER_MANAGER_HPP
