#pragma once

#include "common.h"

#include "material_system.h"
#include "mesh_system.h"
#include "texture_system.h"
#include "transform_system.h"
#include "camera_system.h"

namespace Renderer {
  struct BatchGroup {
    MeshSystem::Mesh *mesh;
    MaterialSystem::Material *material;
    TextureSystem::Texture *texture;
    std::vector<InstanceData> instances;
  };

  void Init();

  void UpdateProjection();

  void SubmitInstanced(
    MeshSystem::Mesh *mesh,
    MaterialSystem::Material *material,
    TextureSystem::Texture *texture,
    const glm::mat4 &modelMatrix,
    const glm::vec4 &color = glm::vec4(1.0f)
  );

  void SetMainCamera(CameraSystem::Camera *camera);

  CameraSystem::Camera *GetMainCamera();

  void Render();

  MeshSystem::Mesh *GetDefaultCubeMesh();

  MaterialSystem::Material *GetDefaultMaterial();

  TextureSystem::Texture *GetDefaultTexture();

  void CleanUp();
};
