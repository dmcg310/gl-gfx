#pragma once

#include "camera_system.h"
#include "common.h"

namespace RenderSystem {
    void Init();

    void SetMainCamera(CameraSystem::Camera *camera);

    CameraSystem::Camera *GetMainCamera();

    void SetClearColor(const glm::vec4 &color);

    glm::vec4 GetClearColor();

    void UpdateProjection();

    void Render(float deltaTime);

    void CleanUp();
}
