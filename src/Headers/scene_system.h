#pragma once

#include "common.h"
#include "light_system.h"
#include "material_system.h"
#include "mesh_system.h"
#include "texture_system.h"
#include "transform_system.h"

namespace SceneSystem {
    struct Entity {
        std::string name;
        mutable MeshSystem::Mesh *mesh = nullptr;
        mutable MaterialSystem::Material *material = nullptr;
        mutable TextureSystem::Texture *texture = nullptr;
        TransformSystem::Transform *transform = nullptr;
        bool isActive = true;
        glm::vec4 color = glm::vec4(1.0f);
    };

    void Init();

    Entity *CreateEntity(const std::string &name, const glm::vec4 &color);

    Entity *CreateLightEntity(const LightSystem::Light *light);

    Entity *GetEntity(const std::string &name);

    const std::vector<Entity *> &GetAllEntities();

    void DestroyEntity(const std::string &name);

    void SetSceneName(const std::string &name);

    std::string GetSceneName();

    void Update();

    void CleanUp();
}
