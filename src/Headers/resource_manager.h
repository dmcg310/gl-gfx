#pragma once

#include "common.h"

#include "shader_system.h"
#include "material_system.h"
#include "mesh_system.h"
#include "texture_system.h"

namespace ResourceManager {
    void Init();

    MeshSystem::Mesh *LoadMesh(const std::string &name, const std::vector<Vertex> &vertices,
                               const std::vector<uint32_t> &indices = std::vector<uint32_t>());

    MeshSystem::Mesh *GetMesh(const std::string &name);

    MeshSystem::Mesh *GetDefaultCubeMesh();

    TextureSystem::Texture *LoadTexture(const std::string &name, const std::string &filePath);

    TextureSystem::Texture *GetTexture(const std::string &name);

    TextureSystem::Texture *GetDefaultTexture();

    ShaderSystem::Shader *LoadShader(const std::string &name,
                                     const std::string &vertexPath,
                                     const std::string &fragmentPath);

    ShaderSystem::Shader *GetShader(const std::string &name);

    ShaderSystem::Shader *GetDefaultShader();

    MaterialSystem::Material *CreateMaterial(const std::string &name, const std::string &shaderName);

    MaterialSystem::Material *CreateDefaultMaterial(const std::string &name);

    MaterialSystem::Material *GetMaterial(const std::string &name);

    MaterialSystem::Material *GetDefaultMaterial();


    void CleanUp();
}
