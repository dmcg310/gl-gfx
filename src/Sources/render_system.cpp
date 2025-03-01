#include "render_system.h"

#include "backend.h"
#include "renderer.h"

namespace RenderSystem {
    void Init() {
        Renderer::Init();
    }

    void SetMainCamera(CameraSystem::Camera *camera) {
        CameraSystem::SetMainCamera(camera);
    }

    CameraSystem::Camera *GetMainCamera() {
        return CameraSystem::GetMainCamera();
    }

    void SetClearColor(const glm::vec4 &color) {
        Renderer::SetClearColor(color);
    }

    glm::vec4 GetClearColor() {
        return Renderer::GetClearColor();
    }

    void UpdateProjection() {
        const float width = Backend::GetWindowWidth();
        const float height = Backend::GetWindowHeight();
        CameraSystem::UpdateMainCameraProjection(width, height);
    }

    void Render(float deltaTime) {
        static float lastWidth = Backend::GetWindowWidth();
        static float lastHeight = Backend::GetWindowHeight();
        const float currentWidth = Backend::GetWindowWidth();
        const float currentHeight = Backend::GetWindowHeight();

        if (currentWidth != lastWidth || currentHeight != lastHeight) {
            UpdateProjection();
            lastWidth = currentWidth;
            lastHeight = currentHeight;
        }

        if (CameraSystem::Camera *mainCamera = CameraSystem::GetMainCamera()) {
            CameraSystem::UpdateCamera(mainCamera, deltaTime);

            const glm::mat4 &viewMatrix = CameraSystem::GetViewMatrix(mainCamera);
            const glm::mat4 &projMatrix = CameraSystem::GetProjectionMatrix(mainCamera);
            const glm::vec3 &cameraPosition = CameraSystem::GetPosition(mainCamera);

            Renderer::Render(viewMatrix, projMatrix, cameraPosition);
        }
    }

    void CleanUp() {
        Renderer::CleanUp();
    }
}
