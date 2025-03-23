#include "api.h"

#include "backend.h"
#include "light_system.h"
#include "render_system.h"
#include "resource_manager.h"
#include "scene_system.h"
#include "serialisation.h"

namespace Api {
    static bool m_isRunning;

    void Init() {
        Backend::Init();
        ResourceManager::Init();
        RenderSystem::Init();
        SceneSystem::Init();
        LightSystem::Init();
    }

    void Run() {
        m_isRunning = true;

        float lastTime = Backend::GetWindowTime();

        while (m_isRunning && Backend::WindowIsOpen()) {
            const float currentTime = Backend::GetWindowTime();
            float deltaTime = currentTime - lastTime;
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

    void Stop() {
        m_isRunning = false;
    }

    bool LoadScene(const std::string& path) {
        return Serialisation::Deserialise(path);
    }

    SceneSystem::Entity* CreateEntity(const std::string& name, const glm::vec4& color) {
        return SceneSystem::CreateEntity(name, color);
    }

    float GetDeltaTime() {
        static float lastTime = Backend::GetWindowTime();
        float currentTime = Backend::GetWindowTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        return deltaTime;
    }
}
