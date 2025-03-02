#include "light_system.h"

#include <unordered_map>
#include <vector>

namespace LightSystem {
    static std::unordered_map<std::string, Light> m_lights;
    static std::vector<Light *> m_lightPtrs;

    void Init() {
        m_lights.clear();
        m_lightPtrs.clear();
    }

    Light *CreateDirectionalLight(const std::string &name, const glm::vec3 &direction,
                                  const glm::vec3 &color, const float intensity) {
        Light light;
        light.name = name;
        light.type = LightType::Directional;
        light.direction = glm::normalize(direction);
        light.color = color;
        light.intensity = intensity;
        light.isActive = true;

        m_lights[name] = light;
        m_lightPtrs.push_back(&m_lights[name]);

        return &m_lights[name];
    }

    Light *CreatePointLight(const std::string &name, const glm::vec3 &position,
                            const glm::vec3 &color, float intensity) {
        Light light;
        light.name = name;
        light.type = LightType::Point;
        light.position = position;
        light.color = color;
        light.intensity = intensity;
        light.isActive = true;

        m_lights[name] = light;
        m_lightPtrs.push_back(&m_lights[name]);

        return &m_lights[name];
    }

    Light *GetLight(const std::string &name) {
        const auto it = m_lights.find(name);
        return (it != m_lights.end()) ? &it->second : nullptr;
    }

    const std::vector<Light *> &GetAllLights() {
        return m_lightPtrs;
    }

    void CleanUp() {
        m_lights.clear();
        m_lightPtrs.clear();
    }
}
