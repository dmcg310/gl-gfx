#pragma once

#include "common.h"
#include "transform_system.h"

namespace CameraSystem {
    struct Camera {
        std::string name;
        TransformSystem::Transform *transform = nullptr;

        glm::vec3 position{0.0f, 0.50f, 3.0f};
        glm::vec3 front{0.0f, 0.0f, -1.0f};
        glm::vec3 up{0.0f, 0.25f, 0.0f};
        glm::vec3 right{1.0f, 0.0f, 0.0f};
        glm::vec3 worldUp{0.0f, 1.0f, 0.0f};

        float yaw{-90.0f};
        float pitch{0.0f};

        float moveSpeed{0.65f};
        float mouseSensitivity{0.1f};
        float zoom{45.0f};

        glm::mat4 viewMatrix{1.0f};
        glm::mat4 projMatrix{1.0f};
        bool viewDirty{true};
        bool projDirty{true};

        float fov{45.0f};
        float aspectRatio{16.0f / 9.0f};
        float nearPlane{0.1f};
        float farPlane{100.0f};
    };

    void Init();

    Camera *CreateCamera(const std::string &name);

    Camera *GetCamera(const std::string &name);

    void UpdateCamera(Camera *camera, float deltaTime);

    void SetProjection(Camera *camera, float fov, float aspect, float nearPlane,
                       float farPlane);

    const glm::mat4 &GetViewMatrix(Camera *camera);

    const glm::mat4 &GetProjectionMatrix(const Camera *camera);

    TransformSystem::Transform *GetTransform(const Camera *camera);

    const glm::vec3 &GetPosition(const Camera *camera);

    void SetMainCamera(Camera *camera);

    Camera *GetMainCamera();

    void UpdateMainCameraProjection(float windowWidth, float windowHeight);

    void CleanUp();
}
