#pragma once

#include "common.h"
#include "shader_system.h"

#include <any>

namespace MaterialSystem {
    struct Property {
        enum class Type {
            Float,
            Vec2,
            Vec3,
            Vec4,
            Mat4,
        };

        Type type;
        std::any value;
    };

    struct Material {
        std::string name;
        ShaderSystem::Shader *shader;
        std::unordered_map<std::string, Property> properties;
    };

    void Init();

    Material *CreateMaterial(const std::string &name, const std::string &shaderName);

    Material *GetMaterial(const std::string &name);

    void SetFloat(Material *material, const std::string &name, float value);

    void SetVec2(Material *material, const std::string &name, const glm::vec2 &value);

    void SetVec3(Material *material, const std::string &name, const glm::vec3 &value);

    void SetVec4(Material *material, const std::string &name, const glm::vec4 &value);

    void SetMat4(Material *material, const std::string &name, const glm::mat4 &value);

    void Bind(const Material *material);

    void Unbind();

    void CleanUp();
}
