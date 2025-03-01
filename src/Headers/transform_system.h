#pragma once

#include "common.h"

namespace TransformSystem {
    struct Transform {
        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};
        bool isDirty{true};
        glm::mat4 modelMatrix{1.0f};
    };

    void Init();

    Transform *CreateTransform(const std::string &name);

    Transform *GetTransform(const std::string &name);

    void SetPosition(Transform *transform, const glm::vec3 &position);

    glm::vec3 GetPosition(const Transform *transform);

    void SetRotation(Transform *transform, const glm::vec3 &rotation);

    glm::vec3 GetRotation(const Transform *transform);

    void SetScale(Transform *transform, const glm::vec3 &scale);

    glm::vec3 GetScale(const Transform *transform);

    const glm::mat4 &GetModelMatrix(Transform *transform);

    void CleanUp();
}
