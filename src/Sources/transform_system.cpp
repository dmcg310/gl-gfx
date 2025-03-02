#include "transform_system.h"

namespace TransformSystem {
    static std::unordered_map<std::string, Transform> m_transforms;

    void Init() {
        m_transforms.clear();
    }

    Transform *CreateTransform(const std::string &name) {
        Transform transform{};
        transform.isDirty = true;

        m_transforms[name] = transform;

        return &m_transforms[name];
    }

    Transform *GetTransform(const std::string &name) {
        const auto it = m_transforms.find(name);
        return (it != m_transforms.end()) ? &it->second : nullptr;
    }

    void SetPosition(Transform *transform, const glm::vec3 &position) {
        if (transform) {
            transform->position = position;
            transform->isDirty = true;
        }
    }

    glm::vec3 GetPosition(const Transform *transform) {
        if (!transform) {
            return glm::vec3{};
        }

        return transform->position;
    }

    void SetRotation(Transform *transform, const glm::vec3 &rotation) {
        if (transform) {
            transform->rotation = rotation;
            transform->isDirty = true;
        }
    }

    glm::vec3 GetRotation(const Transform *transform) {
        if (!transform) {
            return glm::vec3{};
        }

        return transform->rotation;
    }

    void SetScale(Transform *transform, const glm::vec3 &scale) {
        if (transform) {
            transform->scale = scale;
            transform->isDirty = true;
        }
    }

    glm::vec3 GetScale(const Transform *transform) {
        if (!transform) {
            return glm::vec3{};
        }

        return transform->scale;
    }

    const glm::mat4 &GetModelMatrix(Transform *transform) {
        if (!transform) {
            static glm::mat4 identityMatrix(1.0f);
            return identityMatrix;
        }

        if (transform->isDirty) {
            glm::mat4 model(1.0f);

            model = glm::translate(model, transform->position);
            model = glm::rotate(model, glm::radians(transform->rotation.x),
                                glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(transform->rotation.y),
                                glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(transform->rotation.z),
                                glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, transform->scale);

            transform->modelMatrix = model;
            transform->isDirty = false;
        }

        return transform->modelMatrix;
    }

    void CleanUp() {
        m_transforms.clear();
    }
}
