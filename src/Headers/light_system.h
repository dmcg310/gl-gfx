#pragma once

#include "common.h"

namespace LightSystem {
    enum class LightType { Directional, Point };

    struct Light {
        std::string name;
        LightType type;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;
        bool isActive;
    };

    void Init();

    Light *CreateDirectionalLight(const std::string &name, const glm::vec3 &direction,
                                  const glm::vec3 &color = glm::vec3(1.0f),
                                  float intensity = 1.0f);

    Light *CreatePointLight(const std::string &name, const glm::vec3 &position,
                            const glm::vec3 &color = glm::vec3(1.0f),
                            float intensity = 1.0f);

    Light *GetLight(const std::string &name);

    const std::vector<Light *> &GetAllLights();

    void CleanUp();
}
