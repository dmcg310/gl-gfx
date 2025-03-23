#pragma once

#include "common.h"
#include "scene_system.h"

namespace Api {
    void Init();

    void Run();

    void Stop();

    bool LoadScene(const std::string& path);

    SceneSystem::Entity* CreateEntity(const std::string& name, const glm::vec4& color);

    float GetDeltaTime();
}
