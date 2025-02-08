#include "shader_manager.hpp"
#include "error_handler.hpp"

#include <fstream>
#include <iostream>
#include <ostream>

GLuint ShaderManager::create_program(const std::string &vPath,
                                     const std::string &fPath) {
  GLuint vertexShader = load_shader(VERTEX, vPath);
  GLuint fragmentShader = load_shader(FRAGMENT, fPath);

  if (!vertexShader || !fragmentShader) {
    return 0;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  check_compile_errors(program, "PROGRAM");

  glDetachShader(program, vertexShader);
  glDetachShader(program, fragmentShader);

  std::string key = vPath + "_" + fPath;
  m_programs[key] = program;

  return program;
}

GLuint ShaderManager::get_shader(ShaderType type) {
  auto it = m_shaders.find(type);

  if (it != m_shaders.end()) {
    return it->second;
  } else {
    const std::string msg = "Requested shader of type " +
                            shader_type_to_string(type) +
                            " has not been loaded.";

    ErrorHandler::warn(msg, __FILE__, __func__, __LINE__);
    return 0;
  }
}

GLuint ShaderManager::load_shader(ShaderType type, const std::string &path) {
  std::string code;
  std::ifstream file;

  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    file.open(path);

    std::stringstream shaderStream;
    shaderStream << file.rdbuf();

    file.close();

    code = shaderStream.str();
  } catch (std::ifstream::failure e) {
    ErrorHandler::warn("Shader file not successfully read", __FILE__, __func__,
                       __LINE__);
    return 0;
  }

  const char *shaderCode = code.c_str();
  GLuint shaderId = glCreateShader(shader_type_to_gl(type));
  glShaderSource(shaderId, 1, &shaderCode, NULL);
  glCompileShader(shaderId);
  check_compile_errors(shaderId, shader_type_to_string(type));

  m_shaders[type] = shaderId;

  return shaderId;
}

void ShaderManager::check_compile_errors(unsigned int shader,
                                         const std::string &type) {
  int success;
  char infoLog[1024];

  if (type == "PROGRAM") {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "ERROR::PROGRAM_LINKING_ERROR: " << infoLog << std::endl;

      ErrorHandler::throwError("Program linking error", __FILE__, __func__,
                               __LINE__);
    }
  } else {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog << std::endl;

      ErrorHandler::throwError("Shader compilation error", __FILE__, __func__,
                               __LINE__);
    }
  }
}
