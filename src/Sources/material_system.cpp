#include "material_system.h"

namespace MaterialSystem {
    static std::unordered_map<std::string, Material> m_materials;

    void Init() {
        m_materials.clear();
    }

    Material *CreateMaterial(const std::string &name, const std::string &shaderName) {
        auto *shader = ShaderSystem::GetShader(shaderName);
        if (!shader) {
            ErrorHandler::Warn(
                "Failed to create material: shader '" + shaderName + "' not found",
                __FILE__, __func__, __LINE__);
        }

        const Material material{
            .name = name,
            .shader = shader,
            .properties = {},
        };

        m_materials[name] = material;

        return &m_materials[name];
    }

    Material *GetMaterial(const std::string &name) {
        if (const auto it = m_materials.find(name); it != m_materials.end()) {
            return &it->second;
        }

        return nullptr;
    }

    void SetFloat(Material *material, const std::string &name, float value,
                  const bool persistent) {
        material->properties[name] = Property{Property::Type::Float, value, persistent};
    }

    float GetFloat(const Material *material, const std::string &name,
                   float defaultValue) {
        if (!material || material->properties.find(name) == material->properties.end()) {
            return defaultValue;
        }

        const Property &prop = material->properties.at(name);
        if (prop.type != Property::Type::Float) {
            return defaultValue;
        }

        try {
            return std::any_cast<float>(prop.value);
        } catch (const std::bad_any_cast &) {
            return defaultValue;
        }
    }

    void SetInt(Material *material, const std::string &name, int value,
                const bool persistent) {
        material->properties[name] = Property{Property::Type::Int, value, persistent};
    }

    int GetInt(const Material *material, const std::string &name, int defaultValue) {
        if (!material || material->properties.find(name) == material->properties.end()) {
            return defaultValue;
        }

        const Property &prop = material->properties.at(name);
        if (prop.type != Property::Type::Int) {
            return defaultValue;
        }

        try {
            return std::any_cast<int>(prop.value);
        } catch (const std::bad_any_cast &) {
            return defaultValue;
        }
    }

    void SetVec2(Material *material, const std::string &name, const glm::vec2 &value,
                 const bool persistent) {
        material->properties[name] = Property{Property::Type::Vec2, value, persistent};
    }

    void SetVec3(Material *material, const std::string &name, const glm::vec3 &value,
                 const bool persistent) {
        material->properties[name] = Property{Property::Type::Vec3, value, persistent};
    }

    void SetVec4(Material *material, const std::string &name, const glm::vec4 &value,
                 const bool persistent) {
        material->properties[name] = Property{Property::Type::Vec4, value, persistent};
    }

    void SetMat4(Material *material, const std::string &name, const glm::mat4 &value,
                 const bool persistent) {
        material->properties[name] = Property{Property::Type::Mat4, value, persistent};
    }

    void Bind(const Material *material) {
        if (!material || !material->shader) {
            ErrorHandler::Warn("Attempting to bind invalid material", __FILE__, __func__,
                               __LINE__);
            return;
        }

        ShaderSystem::Bind(material->shader);

        for (const auto &[name, prop] : material->properties) {
            try {
                switch (prop.type) {
                    case Property::Type::Float:
                        ShaderSystem::SetFloat(material->shader, name,
                                               std::any_cast<float>(prop.value));
                        break;
                    case Property::Type::Int:
                        ShaderSystem::SetInt(material->shader, name,
                                             std::any_cast<int>(prop.value));
                        break;
                    case Property::Type::Vec2:
                        ShaderSystem::SetVec2(material->shader, name,
                                              std::any_cast<glm::vec2>(prop.value));
                        break;
                    case Property::Type::Vec3:
                        ShaderSystem::SetVec3(material->shader, name,
                                              std::any_cast<glm::vec3>(prop.value));
                        break;
                    case Property::Type::Vec4:
                        ShaderSystem::SetVec4(material->shader, name,
                                              std::any_cast<glm::vec4>(prop.value));
                        break;
                    case Property::Type::Mat4:
                        ShaderSystem::SetMat4(material->shader, name,
                                              std::any_cast<glm::mat4>(prop.value));
                        break;
                }
            } catch (const std::bad_any_cast &e) {
                ErrorHandler::Warn("Failed to set material property: " + name, __FILE__,
                                   __func__, __LINE__);
            }
        }
    }

    void Unbind() {
        ShaderSystem::Unbind();
    }

    void CleanUp() {
        m_materials.clear();
    }
}
