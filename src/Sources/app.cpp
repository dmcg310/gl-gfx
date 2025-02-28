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

        auto *sharedMaterial = ResourceManager::CreateDefaultMaterial("shared_material");
        MaterialSystem::SetInt(sharedMaterial, "useInstanceColor", 1);

        std::vector<SceneSystem::Entity *> allCubes;
        constexpr int gridSize = 20; // gridSize x gridSize x gridSize
        constexpr float spacing = 0.8f;

        for (int x = 0; x < gridSize; x++) {
            for (int y = 0; y < gridSize; y++) {
                for (int z = 0; z < gridSize; z++) {
                    const float xPos = (x - gridSize / 2) * spacing;
                    const float yPos = (y - gridSize / 2) * spacing;
                    const float zPos = (z - gridSize / 2) * spacing;

                    glm::vec4 cubeColor(
                        0.5f + 0.5f * (x / float(gridSize)),
                        0.5f + 0.5f * (y / float(gridSize)),
                        0.5f + 0.5f * (z / float(gridSize)),
                        1.0f
                    );

                    std::string cubeName = "cube_" + std::to_string(x) + "_" + std::to_string(y) + "_" +
                                           std::to_string(z);

                    auto *cube = SceneSystem::CreateEntity(cubeName, cubeColor);
                    cube->mesh = ResourceManager::GetDefaultCubeMesh();
                    cube->material = sharedMaterial;
                    cube->texture = ResourceManager::GetDefaultTexture();

                    TransformSystem::SetPosition(cube->transform, glm::vec3(xPos, yPos, zPos));
                    TransformSystem::SetScale(cube->transform, glm::vec3(0.35f));
                    allCubes.push_back(cube);
                }
            }
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
        Backend::CleanUp();
    }
}
