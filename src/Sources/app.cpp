#include "app.h"

#include "scene_system.h"

#include <iostream>
#include <chrono>

namespace App {
    float frameTimeAccumulator = 0.0f;
    int frameCount = 0;

    void Run() {
        Backend::Init();
        Renderer::Init();
        SceneSystem::Init();

        std::vector<SceneSystem::Entity *> allCubes;

        auto *sharedMaterial = MaterialSystem::CreateDefaultMaterial("shared_material");
        MaterialSystem::SetInt(sharedMaterial, "useInstanceColor", 1);

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
                    cube->mesh = Renderer::GetDefaultCubeMesh();
                    cube->material = sharedMaterial;
                    cube->texture = Renderer::GetDefaultTexture();

                    TransformSystem::SetPosition(cube->transform, glm::vec3(xPos, yPos, zPos));
                    TransformSystem::SetScale(cube->transform, glm::vec3(0.35f));
                    allCubes.push_back(cube);
                }
            }
        }

        float animationTime = 0.0f;
        float lastTime = Backend::GetWindowTime();
        float deltaTime = 0.0f;

        while (Backend::WindowIsOpen()) {
            const float currentTime = Backend::GetWindowTime();
            deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            animationTime += deltaTime;
            frameTimeAccumulator += deltaTime;
            frameCount++;

            if (frameTimeAccumulator >= 1.0f) {
                const float fps = frameCount / frameTimeAccumulator;
                std::cout << "FPS: " << fps << " (" << allCubes.size() << " cubes)" << std::endl;
                frameTimeAccumulator = 0.0f;
                frameCount = 0;
            }

            Backend::BeginFrame();

            Backend::Update();
            SceneSystem::Update();

            Renderer::Render();

            Backend::EndFrame();
        }

        SceneSystem::CleanUp();
        Renderer::CleanUp();
        Backend::CleanUp();
    }
}
