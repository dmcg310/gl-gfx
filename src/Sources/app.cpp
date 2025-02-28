#include "app.h"

#include "backend.h"
#include "render_system.h"
#include "resource_manager.h"
#include "scene_system.h"

#include <chrono>

namespace App {
    void Run() {
        Backend::Init();
        ResourceManager::Init();
        RenderSystem::Init();
        SceneSystem::Init();

        const auto *camera = RenderSystem::GetMainCamera();
        auto *cameraTransform = CameraSystem::GetTransform(camera);
        TransformSystem::SetPosition(cameraTransform, glm::vec3(0.0f, 15.0f, 30.0f));
        TransformSystem::SetRotation(cameraTransform, glm::vec3(-15.0f, 0.0f, 0.0f));

        auto *cube = SceneSystem::CreateEntity("cube", glm::vec4(1.0f));
        cube->mesh = ResourceManager::GetDefaultCubeMesh();
        cube->material = ResourceManager::GetDefaultMaterial();
        cube->texture = ResourceManager::GetDefaultTexture();

        TransformSystem::SetPosition(cube->transform, glm::vec3(0.0f, 0.0f, 0.0f));
        TransformSystem::SetScale(cube->transform, glm::vec3(0.35f));


        float lastTime = Backend::GetWindowTime();
        float deltaTime = 0.0f;

        while (Backend::WindowIsOpen()) {
            const float currentTime = Backend::GetWindowTime();
            deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            if (Backend::WindowIsMinimized()) {
                continue;
            }

            Backend::BeginFrame();

            Backend::Update();
            SceneSystem::Update();

            Backend::PrepareUi();

            RenderSystem::Render(deltaTime);
            Backend::RenderUi();

            Backend::EndFrame();
        }

        SceneSystem::CleanUp();
        RenderSystem::CleanUp();
        ResourceManager::CleanUp();
        Backend::CleanUp();
    }
}
