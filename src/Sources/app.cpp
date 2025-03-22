#include "app.h"

#include "backend.h"
#include "light_system.h"
#include "render_system.h"
#include "resource_manager.h"
#include "scene_system.h"
#include "serialisation.h"

namespace App {
    void Run() {
        Backend::Init();
        ResourceManager::Init();
        RenderSystem::Init();
        SceneSystem::Init();
        LightSystem::Init();

        Serialisation::Deserialise("demo");

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

            if (g_EnableDebugFeatures) {
                Backend::PrepareUi();
            }

            RenderSystem::Render(deltaTime);

            if (g_EnableDebugFeatures) {
                Backend::RenderUi();
            }

            Backend::EndFrame();
        }

        SceneSystem::CleanUp();
        RenderSystem::CleanUp();
        ResourceManager::CleanUp();
        LightSystem::CleanUp();
        Backend::CleanUp();
    }
}
