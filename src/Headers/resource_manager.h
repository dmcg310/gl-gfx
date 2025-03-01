#pragma once

#include "common.h"

#include "shader_system.h"
#include "material_system.h"
#include "mesh_system.h"
#include "texture_system.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ResourceManager {
    void Init();

    MeshSystem::Mesh *LoadMesh(const std::string &name, const std::string &filePath);

    MeshSystem::Mesh *GetMesh(const std::string &name);

    TextureSystem::Texture *LoadTexture(const std::string &name, const std::string &filePath, bool generateMips = true);

    TextureSystem::Texture *GetTexture(const std::string &name);

    ShaderSystem::Shader *LoadShader(const std::string &name, const std::string &vertPath, const std::string &fragPath);

    ShaderSystem::Shader *GetShader(const std::string &name);

    MaterialSystem::Material *CreateMaterial(const std::string &name, const std::string &shaderName,
                                             bool useTexture = false);

    MaterialSystem::Material *GetMaterial(const std::string &name);

    MeshSystem::Mesh *GetDefaultCubeMesh();

    MeshSystem::Mesh *GetDefaultPlaneMesh();

    TextureSystem::Texture *GetDefaultTexture();

    ShaderSystem::Shader *GetDefaultShader();

    MaterialSystem::Material *GetDefaultMaterial();

    MeshSystem::Mesh *CreateDefaultCubeMesh();

    MeshSystem::Mesh *CreateDefaultPlaneMesh();

    TextureSystem::Texture *CreateDefaultTexture();

    ShaderSystem::Shader *CreateDefaultShader();

    MaterialSystem::Material *CreateDefaultMaterial();

    void ProcessAssimpMesh(const aiScene *scene, const aiMesh *mesh, std::vector<Vertex> &vertices,
                           std::vector<uint32_t> &indices);

    bool LoadMeshDataFromFile(const std::string &filePath, std::vector<Vertex> &vertices,
                              std::vector<uint32_t> &indices);

    void CleanUp();
}
