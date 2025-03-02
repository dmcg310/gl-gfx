#pragma once

#include "common.h"

namespace ShaderSystem {
    struct Shader {
        GLuint programId;
        std::string vertPath;
        std::string fragPath;
        std::string name;
        bool isValid;
    };

    void Init();

    Shader *CreateShader(const std::string &name, const std::string &vertPath,
                         const std::string &fragPath);

    Shader *GetShader(const std::string &name);

    void Bind(const Shader *shader);

    void Unbind();

    void SetFloat(const Shader *shader, const std::string &name, float value);

    void SetInt(const Shader *shader, const std::string &name, int value);

    void SetVec2(const Shader *shader, const std::string &name, const glm::vec2 &value);

    void SetVec3(const Shader *shader, const std::string &name, const glm::vec3 &value);

    void SetVec4(const Shader *shader, const std::string &name, const glm::vec4 &value);

    void SetMat4(const Shader *shader, const std::string &name, const glm::mat4 &value);

    void CleanUp();
}
