#pragma once

#include "common.h"

namespace ShaderManager {
    GLuint CreateProgram(const std::string &vPath, const std::string &fPath);

    GLuint LoadShader(ShaderType type, const std::string &path);

    std::string GetShaderPath(const std::string &filename);

    void CleanUp();

    inline GLenum ShaderTypeToGL(ShaderType type) {
        return type == VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    }

    inline std::string ShaderTypeToString(ShaderType type) {
        return type == VERTEX ? "VERTEX" : "FRAGMENT";
    }
}
