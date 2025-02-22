#include "shader_manager.h"

#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace ShaderManager {
    std::unordered_map<ShaderType, unsigned int> m_shaders;
    std::unordered_map<std::string, unsigned int> m_programs;

    static GLuint LoadShader(ShaderType type, const std::string &path) {
        std::string code;

        try {
            std::ifstream file;

            file.open(path);
            if (!file.is_open()) {
                ErrorHandler::Warn("Could not open shader file: " + path, __FILE__,
                                   __func__, __LINE__);
                return 0;
            }

            std::stringstream shaderStream;
            shaderStream << file.rdbuf();
            file.close();
            code = shaderStream.str();
        } catch (std::ifstream::failure /*e*/) {
            ErrorHandler::Warn("Shader file not successfully read: " + path, __FILE__,
                               __func__, __LINE__);
            return 0;
        }

        const char *shaderCode = code.c_str();
        GLuint shaderId = glCreateShader(ShaderTypeToGL(type));
        glShaderSource(shaderId, 1, &shaderCode, nullptr);
        glCompileShader(shaderId);

        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetShaderInfoLog(shaderId, 1024, nullptr, infoLog);
            ErrorHandler::ThrowError(
                "Shader compilation error of type " + ShaderTypeToString(type) + ": " + std::string(infoLog),
                __FILE__, __func__, __LINE__);
        }

        m_shaders[type] = shaderId;
        return shaderId;
    }

    static std::string GetShaderPath(const std::string &filename) {
        std::filesystem::path paths[] = {
            "src/Shaders",
            "../src/Shaders",
            "../../src/Shaders",
            "gl-gfx/src/Shaders"
        };

        for (const auto &basePath: paths) {
            if (std::filesystem::path fullPath = basePath / filename; exists(fullPath)) {
                return fullPath.string();
            }
        }

        return filename;
    }

    GLuint CreateProgram(const std::string &vPath, const std::string &fPath) {
        const std::filesystem::path vertexFile = std::filesystem::path(vPath).filename();
        const std::filesystem::path fragmentFile = std::filesystem::path(fPath).filename();

        const GLuint vertexShader = LoadShader(VERTEX, GetShaderPath(vertexFile.string()));
        const GLuint fragmentShader = LoadShader(FRAGMENT, GetShaderPath(fragmentFile.string()));
        if (!vertexShader || !fragmentShader) {
            return 0;
        }

        const GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(program, 1024, nullptr, infoLog);
            ErrorHandler::ThrowError(std::string("Program linking error: ") + infoLog,
                                     __FILE__, __func__, __LINE__);
        }

        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);

        const std::string key = vPath + "_" + fPath;
        m_programs[key] = program;

        return program;
    }

    void CleanUp() {
        for (const auto &[_, shader]: m_shaders) {
            glDeleteShader(shader);
        }

        for (const auto &[_, program]: m_programs) {
            glDeleteProgram(program);
        }

        m_shaders.clear();
        m_programs.clear();
    }
}
