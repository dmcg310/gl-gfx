#pragma once

#include <any>

#include "common.h"
#include "shader_system.h"

namespace MaterialSystem {
    struct Property {
        enum class Type {
            Float,
            Int,
            Vec2,
            Vec3,
            Vec4,
            Mat4,
        };

        Type type;
        std::any value;
        bool persistent = true;
    };

    struct Material {
        std::string name;
        ShaderSystem::Shader *shader;
        std::unordered_map<std::string, Property> properties;
    };

    void Init();

    Material *CreateMaterial(const std::string &name, const std::string &shaderName);

    Material *GetMaterial(const std::string &name);

    void SetFloat(Material *material, const std::string &name, float value,
                  bool persistent = true);

    float GetFloat(const Material *material, const std::string &name,
                   float defaultValue = 0.0f);

    void SetInt(Material *material, const std::string &name, int value,
                bool persistent = true);

    int GetInt(const Material *material, const std::string &name, int defaultValue = 0);

    void SetVec2(Material *material, const std::string &name, const glm::vec2 &value,
                 bool persistent = true);

    void SetVec3(Material *material, const std::string &name, const glm::vec3 &value,
                 bool persistent = true);

    void SetVec4(Material *material, const std::string &name, const glm::vec4 &value,
                 bool persistent = true);

    void SetMat4(Material *material, const std::string &name, const glm::mat4 &value,
                 bool persistent = true);

    void Bind(const Material *material);

    void Unbind();

    void CleanUp();
}
