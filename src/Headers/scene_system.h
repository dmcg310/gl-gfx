#pragma once

#include "mesh_system.h"
#include "material_system.h"
#include "texture_system.h"
#include "transform_system.h"

#include "common.h"

namespace SceneSystem {
    struct Entity {
        std::string name;
        MeshSystem::Mesh *mesh = nullptr;
        MaterialSystem::Material *material = nullptr;
        TextureSystem::Texture *texture = nullptr;
        TransformSystem::Transform *transform = nullptr;
        bool isActive = true;
    };

    void Init();

    Entity *CreateEntity(const std::string &name);

    Entity *GetEntity(const std::string &name);

    const std::vector<Entity *> &GetAllEntities();

    void DestroyEntity(const std::string &name);

    void Update();

    void CleanUp();
}
