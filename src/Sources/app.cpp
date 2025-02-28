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

        auto *floor = SceneSystem::CreateEntity("floor", glm::vec4(1.0f));
        floor->mesh = ResourceManager::GetDefaultPlaneMesh();
        floor->material = ResourceManager::GetDefaultMaterial();
        floor->texture = ResourceManager::GetDefaultTexture();
        TransformSystem::SetScale(floor->transform, glm::vec3(100.0f, 1.0f, 100.0f));
        TransformSystem::SetPosition(floor->transform, glm::vec3(0.0f, floor->mesh->minBounds.y, 0.0f));

        auto *cube = SceneSystem::CreateEntity("cube", glm::vec4(0.5f));
        cube->mesh = ResourceManager::GetDefaultCubeMesh();
        cube->material = ResourceManager::GetDefaultMaterial();
        cube->texture = ResourceManager::GetDefaultTexture();
        const float cubeHeightOffset = (cube->mesh->maxBounds.y - cube->mesh->minBounds.y) * 0.5f;
        TransformSystem::SetPosition(cube->transform, glm::vec3(0.0f, cubeHeightOffset, 0.0f));

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
