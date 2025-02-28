#include "resource_manager.h"

#include <filesystem>

namespace ResourceManager {
    std::unordered_map<std::string, MeshSystem::Mesh> m_meshes;
    std::unordered_map<std::string, TextureSystem::Texture> m_textures;
    std::unordered_map<std::string, ShaderSystem::Shader> m_shaders;
    std::unordered_map<std::string, MaterialSystem::Material> m_materials;

    MeshSystem::Mesh *m_defaultCubeMesh = nullptr;
    TextureSystem::Texture *m_defaultTexture = nullptr;
    ShaderSystem::Shader *m_defaultShader = nullptr;
    MaterialSystem::Material *m_defaultMaterial = nullptr;

    void Init() {
        MeshSystem::Init();
        TextureSystem::Init();
        ShaderSystem::Init();
        MaterialSystem::Init();

        m_defaultCubeMesh = CreateDefaultCubeMesh();
        m_defaultTexture = CreateDefaultTexture();
        m_defaultShader = CreateDefaultShader();
        m_defaultMaterial = CreateDefaultMaterial();
    }

    MeshSystem::Mesh *LoadMesh(const std::string &name, const std::string &filePath) {
        if (const auto it = m_meshes.find(name); it != m_meshes.end()) {
            return &it->second;
        }

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        if (!LoadMeshDataFromFile(filePath, vertices, indices)) {
            ErrorHandler::Warn("Failed to load mesh: " + filePath + ". Using default cube.", __FILE__, __func__,
                               __LINE__);

            return m_defaultCubeMesh;
        }

        const MeshSystem::Mesh *mesh = MeshSystem::CreateMesh(name, vertices, indices);
        if (!mesh) {
            ErrorHandler::Warn("Failed to create mesh: " + name + ". Using default cube.", __FILE__, __func__,
                               __LINE__);

            return m_defaultCubeMesh;
        }

        m_meshes[name] = *mesh;

        return &m_meshes[name];
    }

    MeshSystem::Mesh *GetMesh(const std::string &name) {
        if (const auto it = m_meshes.find(name); it != m_meshes.end()) {
            return &it->second;
        }

        ErrorHandler::Warn("Mesh not found: " + name + ". Using default cube.", __FILE__, __func__, __LINE__);

        return m_defaultCubeMesh;
    }

    TextureSystem::Texture *LoadTexture(const std::string &name, const std::string &filePath, bool generateMips) {
        if (const auto it = m_textures.find(name); it != m_textures.end()) {
            return &it->second;
        }

        const TextureSystem::Texture *texture = TextureSystem::CreateTexture(name, filePath, generateMips);
        if (!texture) {
            ErrorHandler::Warn("Failed to load texture: " + filePath + ". Using default texture.", __FILE__, __func__,
                               __LINE__);

            return m_defaultTexture;
        }

        m_textures[name] = *texture;

        return &m_textures[name];
    }

    TextureSystem::Texture *GetTexture(const std::string &name) {
        if (const auto it = m_textures.find(name); it != m_textures.end()) {
            return &it->second;
        }

        ErrorHandler::Warn("Texture not found: " + name + ". Using default texture.", __FILE__, __func__, __LINE__);

        return m_defaultTexture;
    }

    ShaderSystem::Shader *LoadShader(const std::string &name, const std::string &vertPath,
                                     const std::string &fragPath) {
        if (const auto it = m_shaders.find(name); it != m_shaders.end()) {
            return &it->second;
        }

        ShaderSystem::Shader *shader = ShaderSystem::CreateShader(name, vertPath, fragPath);
        if (!shader) {
            ErrorHandler::Warn("Failed to load shader: " + name + ". Using default shader.", __FILE__, __func__,
                               __LINE__);

            return m_defaultShader;
        }

        m_shaders[name] = *shader;

        return shader;
    }

    ShaderSystem::Shader *GetShader(const std::string &name) {
        if (const auto it = m_shaders.find(name); it != m_shaders.end()) {
            return &it->second;
        }

        ErrorHandler::Warn("Shader not found: " + name + ". Using default shader.", __FILE__, __func__,
                           __LINE__);

        return m_defaultShader;
    }

    MaterialSystem::Material *CreateMaterial(const std::string &name, const std::string &shaderName) {
        if (const auto it = m_materials.find(name); it != m_materials.end()) {
            return &it->second;
        }

        MaterialSystem::Material *material = MaterialSystem::CreateMaterial(name, shaderName);
        if (!material) {
            ErrorHandler::Warn("Could not create material: " + name + ". Using default material.", __FILE__, __func__,
                               __LINE__);

            return m_defaultMaterial;
        }

        SetVec3(material, "color", glm::vec3(1.0f));
        SetInt(material, "mainTexture", 0);
        SetInt(material, "useTexture", 0);

        m_materials[name] = *material;

        return &m_materials[name];
    }

    MaterialSystem::Material *GetMaterial(const std::string &name) {
        if (const auto it = m_materials.find(name); it != m_materials.end()) {
            return &it->second;
        }

        ErrorHandler::Warn("Material not found: " + name + ".  Using default material.", __FILE__, __func__, __LINE__);

        return m_defaultMaterial;
    }

    MeshSystem::Mesh *GetDefaultCubeMesh() {
        return m_defaultCubeMesh;
    }

    TextureSystem::Texture *GetDefaultTexture() {
        return m_defaultTexture;
    }

    ShaderSystem::Shader *GetDefaultShader() {
        return m_defaultShader;
    }

    MaterialSystem::Material *GetDefaultMaterial() {
        return m_defaultMaterial;
    }

    MeshSystem::Mesh *CreateDefaultCubeMesh() {
        return LoadMesh("default", "../Assets/Models/cube.obj");
    }

    TextureSystem::Texture *CreateDefaultTexture() {
        return LoadTexture("default", "../Assets/Textures/default.png");
    }

    ShaderSystem::Shader *CreateDefaultShader() {
        return LoadShader("default", "../src/Shaders/default.vert", "../src/Shaders/default.frag");
    }

    MaterialSystem::Material *CreateDefaultMaterial() {
        return CreateMaterial("default", "default");
    }

    void ProcessAssimpMesh(const aiScene */*scene*/, const aiMesh *mesh, std::vector<Vertex> &vertices,
                           std::vector<uint32_t> &indices) {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};
            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            if (mesh->HasNormals()) {
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            }

            if (mesh->mTextureCoords[0]) {
                vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            } else {
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            const aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    bool LoadMeshDataFromFile(const std::string &filePath, std::vector<Vertex> &vertices,
                              std::vector<uint32_t> &indices) {
        const std::filesystem::path meshFile = std::filesystem::path(filePath).filename();
        std::filesystem::path paths[] = {
            "Assets/Models",
            "../Assets/Models",
            "../../Assets/Models",
            "gl-gfx/Assets/Models"
        };

        std::string trueFilePath;
        for (const auto &basePath: paths) {
            if (std::filesystem::path fullPath = basePath / meshFile; exists(fullPath)) {
                trueFilePath = fullPath.string();
                break;
            }
        }

        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(trueFilePath,
                                                 aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            ErrorHandler::Warn("ERROR::ASSIMP:: " + std::string(importer.GetErrorString()), __FILE__, __func__,
                               __LINE__);

            return false;
        }

        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh *mesh = scene->mMeshes[i];
            ProcessAssimpMesh(scene, mesh, vertices, indices);
        }

        return true;
    }

    void CleanUp() {
        MeshSystem::CleanUp();
        TextureSystem::CleanUp();
        ShaderSystem::CleanUp();
        MaterialSystem::CleanUp();

        m_meshes.clear();
        m_textures.clear();
        m_shaders.clear();

        m_defaultCubeMesh = nullptr;
        m_defaultTexture = nullptr;
        m_defaultShader = nullptr;
        m_defaultMaterial = nullptr;
    }
}
