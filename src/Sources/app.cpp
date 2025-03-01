#include "app.h"

#include "backend.h"
#include "render_system.h"
#include "resource_manager.h"
#include "scene_system.h"
#include "light_system.h"

#include <chrono>

namespace App {
    void Run() {
        Backend::Init();
        ResourceManager::Init();
        RenderSystem::Init();
        SceneSystem::Init();
        LightSystem::Init();

        SceneSystem::CreateLightEntity(LightSystem::CreatePointLight(
            "pointLight",
            glm::vec3(0.0f, 2.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            3.0f
        ));

        auto *floor = SceneSystem::CreateEntity("floor", glm::vec4(0.5f));
        floor->mesh = ResourceManager::GetDefaultPlaneMesh();
        floor->material = ResourceManager::GetDefaultMaterial();
        floor->texture = ResourceManager::GetDefaultTexture();
        TransformSystem::SetScale(floor->transform, glm::vec3(100.0f, 1.0f, 100.0f));
        TransformSystem::SetPosition(floor->transform, glm::vec3(0.0f, floor->mesh->minBounds.y, 0.0f));

        for (int i = 0; i < 5; i++) {
            auto *cube = SceneSystem::CreateEntity("cube" + std::to_string(i + 1), glm::vec4(1.0f));
            cube->mesh = ResourceManager::GetDefaultCubeMesh();
            cube->material = ResourceManager::CreateMaterial("container" + std::to_string(i + 1), "default", true);
            cube->texture = ResourceManager::LoadTexture("container", "../Assets/Textures/container.png");

            const float angle = (2.0f * glm::pi<float>() * i) / 5.0f;
            const float x = 3.0f * cos(angle);
            const float z = 3.0f * sin(angle);

            const float cubeHeightOffset = (cube->mesh->maxBounds.y - cube->mesh->minBounds.y) * 0.5f;
            TransformSystem::SetPosition(cube->transform, glm::vec3(x, cubeHeightOffset, z));
        }

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
        LightSystem::CleanUp();
        Backend::CleanUp();
    }
}
