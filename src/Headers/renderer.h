#pragma once

#include "common.h"

#include "material_system.h"
#include "mesh_system.h"
#include "texture_system.h"
#include "transform_system.h"
#include "camera_system.h"

namespace Renderer {
  struct RenderCommand {
    MeshSystem::Mesh *mesh;
    MaterialSystem::Material *material;
    TextureSystem::Texture *texture;
    glm::mat4 modelMatrix;

    bool operator<(const RenderCommand &other) const {
      if (material != other.material) {
        return material < other.material;
      }

      if (texture != other.texture) {
        return texture < other.texture;
      }

      return mesh < other.mesh;
    }
  };

  void Init();

  void UpdateProjection();

  void Submit(
    MeshSystem::Mesh *mesh,
    MaterialSystem::Material *material,
    TextureSystem::Texture *texture,
    TransformSystem::Transform *transform
  );

  void ClearQueue();

  void SetMainCamera(CameraSystem::Camera *camera);

  CameraSystem::Camera *GetMainCamera();

  void Render();

  MeshSystem::Mesh *GetDefaultCubeMesh();

  MaterialSystem::Material *GetDefaultMaterial();

  TextureSystem::Texture *GetDefaultTexture();

  void CleanUp();
};
