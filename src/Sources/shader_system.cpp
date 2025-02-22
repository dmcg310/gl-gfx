#include "shader_system.h"

#include "shader_manager.h"

namespace ShaderSystem {
    static std::unordered_map<std::string, Shader> m_shaders;
    static std::unordered_map<std::string, GLint> m_uniformLocations;

    void Init() {
        m_shaders.clear();
        m_uniformLocations.clear();
    }

    Shader *CreateShader(const std::string &name, const std::string &vertPath, const std::string &fragPath) {
        Shader shader{
            .programId = ShaderManager::CreateProgram(vertPath, fragPath),
            .vertPath = vertPath,
            .fragPath = fragPath,
            .isValid = false,
        };

        shader.isValid = (shader.programId != 0);
        if (shader.isValid) {
            m_shaders[name] = shader;
            return &m_shaders[name];
        }

        return nullptr;
    }

    Shader *GetShader(const std::string &name) {
        const auto it = m_shaders.find(name);
        return (it != m_shaders.end()) ? &it->second : nullptr;
    }

    void Bind(const Shader *shader) {
        if (shader && shader->isValid) {
            glUseProgram(shader->programId);
        }
    }

    void Unbind() {
        glUseProgram(0);
    }

    static GLint GetUniformLocation(const Shader *shader, const std::string &name) {
        const std::string key = std::to_string(shader->programId) + "_" + name;
        if (const auto it = m_uniformLocations.find(key); it != m_uniformLocations.end()) {
            return it->second;
        }

        const GLint location = glGetUniformLocation(shader->programId, name.c_str());
        if (location == -1) {
            ErrorHandler::Warn("Uniform '" + name + "' not found in shader program", __FILE__, __func__, __LINE__);
        }

        m_uniformLocations[key] = location;

        return location;
    }

    void SetFloat(const Shader *shader, const std::string &name, const float value) {
        if (const GLint location = GetUniformLocation(shader, name); location != -1) {
            glUniform1f(location, value);
        }
    }

    void SetInt(const Shader *shader, const std::string &name, const int value) {
        if (const GLint location = GetUniformLocation(shader, name); location != -1) {
            glUniform1i(location, value);
        }
    }

    void SetVec2(const Shader *shader, const std::string &name, const glm::vec2 &value) {
        if (const GLint location = GetUniformLocation(shader, name); location != -1) {
            glUniform2fv(location, 1, glm::value_ptr(value));
        }
    }

    void SetVec3(const Shader *shader, const std::string &name, const glm::vec3 &value) {
        if (const GLint location = GetUniformLocation(shader, name); location != -1) {
            glUniform3fv(location, 1, glm::value_ptr(value));
        }
    }

    void SetVec4(const Shader *shader, const std::string &name, const glm::vec4 &value) {
        if (const GLint location = GetUniformLocation(shader, name); location != -1) {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
    }

    void SetMat4(const Shader *shader, const std::string &name, const glm::mat4 &value) {
        if (const GLint location = GetUniformLocation(shader, name); location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void CleanUp() {
        m_shaders.clear();
        m_uniformLocations.clear();

        ShaderManager::CleanUp();
    }
}
