#include "app.h"

#include "backend.h"
#include "resource_manager.h"
#include "renderer.h"
#include "scene_system.h"

#include <chrono>

namespace App {
    void Run() {
        Backend::Init();
        ResourceManager::Init();
        Renderer::Init();
        SceneSystem::Init();

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

        while (Backend::WindowIsOpen()) {
            if (Backend::WindowIsMinimized()) {
                continue;
            }

            Backend::BeginFrame();

            Backend::Update();
            SceneSystem::Update();

            Backend::PrepareUi();

            Renderer::Render();
            Backend::RenderUi();

            Backend::EndFrame();
        }

        SceneSystem::CleanUp();
        Renderer::CleanUp();
        ResourceManager::CleanUp();
        Backend::CleanUp();
    }
}
