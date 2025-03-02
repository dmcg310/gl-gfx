#pragma once

#include "material_system.h"
#include "mesh_system.h"
#include "texture_system.h"

namespace Renderer {
    struct BatchGroup {
        MeshSystem::Mesh *mesh;
        MaterialSystem::Material *material;
        TextureSystem::Texture *texture;
        std::vector<InstanceData> instances;
    };

    void Init();

    void SetClearColor(const glm::vec4 &color);

    glm::vec4 GetClearColor();

    void SubmitInstanced(MeshSystem::Mesh *mesh, MaterialSystem::Material *material,
                         TextureSystem::Texture *texture, const glm::mat4 &modelMatrix,
                         const glm::vec4 &color = glm::vec4(1.0f));

    void Render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix,
                const glm::vec3 &cameraPosition);

    void CleanUp();
};
