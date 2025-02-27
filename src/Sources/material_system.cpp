#include "material_system.h"

namespace MaterialSystem {
    static std::unordered_map<std::string, Material> m_materials;

    void Init() {
        m_materials.clear();
    }

    Material *CreateMaterial(const std::string &name, const std::string &shaderName) {
        auto *shader = ShaderSystem::GetShader(shaderName);
        if (!shader) {
            ErrorHandler::Warn("Failed to create material: shader '" + shaderName + "' not found", __FILE__,
                               __func__, __LINE__
            );
        }

        const Material material{
            .name = name,
            .shader = shader,
            .properties = {},
        };

        m_materials[name] = material;

        return &m_materials[name];
    }

    Material *CreateDefaultMaterial(const std::string &name) {
        Material *material = CreateMaterial(name, "default");
        if (!material) {
            ErrorHandler::Warn("Failed to create default material", __FILE__, __func__, __LINE__);
            return nullptr;
        }

        SetVec3(material, "color", glm::vec3(1.0f));
        SetInt(material, "mainTexture", 0);
        SetInt(material, "useTexture", 1);

        return material;
    }

    Material *GetMaterial(const std::string &name) {
        if (const auto it = m_materials.find(name); it != m_materials.end()) {
            return &it->second;
        }

        return nullptr;
    }

    void SetFloat(Material *material, const std::string &name, float value) {
        material->properties[name] = Property{Property::Type::Float, value};
    }

    void SetInt(Material *material, const std::string &name, int value) {
        material->properties[name] = Property{Property::Type::Int, value};
    }

    void SetVec2(Material *material, const std::string &name, const glm::vec2 &value) {
        material->properties[name] = Property{Property::Type::Vec2, value};
    }

    void SetVec3(Material *material, const std::string &name, const glm::vec3 &value) {
        material->properties[name] = Property{Property::Type::Vec3, value};
    }

    void SetVec4(Material *material, const std::string &name, const glm::vec4 &value) {
        material->properties[name] = Property{Property::Type::Vec4, value};
    }

    void SetMat4(Material *material, const std::string &name, const glm::mat4 &value) {
        material->properties[name] = Property{Property::Type::Mat4, value};
    }

    void Bind(const Material *material) {
        if (!material || !material->shader) {
            ErrorHandler::Warn("Attempting to bind invalid material", __FILE__, __func__, __LINE__);
            return;
        }

        ShaderSystem::Bind(material->shader);

        for (const auto &[name, prop]: material->properties) {
            try {
                switch (prop.type) {
                    case Property::Type::Float:
                        ShaderSystem::SetFloat(material->shader, name, std::any_cast<float>(prop.value));
                        break;
                    case Property::Type::Int:
                        ShaderSystem::SetInt(material->shader, name, std::any_cast<int>(prop.value));
                        break;
                    case Property::Type::Vec2:
                        ShaderSystem::SetVec2(material->shader, name, std::any_cast<glm::vec2>(prop.value));
                        break;
                    case Property::Type::Vec3:
                        ShaderSystem::SetVec3(material->shader, name, std::any_cast<glm::vec3>(prop.value));
                        break;
                    case Property::Type::Vec4:
                        ShaderSystem::SetVec4(material->shader, name, std::any_cast<glm::vec4>(prop.value));
                        break;
                    case Property::Type::Mat4:
                        ShaderSystem::SetMat4(material->shader, name, std::any_cast<glm::mat4>(prop.value));
                        break;
                }
            } catch (const std::bad_any_cast &e) {
                ErrorHandler::Warn("Failed to set material property: " + name, __FILE__, __func__, __LINE__
                );
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
