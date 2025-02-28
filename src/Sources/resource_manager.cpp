#include "resource_manager.h"

namespace ResourceManager {
    static ShaderSystem::Shader *m_defaultShader = nullptr;
    static MaterialSystem::Material *m_defaultMaterial = nullptr;
    static MeshSystem::Mesh *m_defaultCubeMesh = nullptr;
    static TextureSystem::Texture *m_defaultTexture = nullptr;

    void Init() {
        ShaderSystem::Init();
        MaterialSystem::Init();
        MeshSystem::Init();
        TextureSystem::Init();

        m_defaultShader = LoadShader("default", "../src/Shaders/default.vert", "../src/Shaders/default.frag");
        if (!m_defaultShader) {
            ErrorHandler::ThrowError("Failed to create default shader", __FILE__, __func__, __LINE__);
        }

        m_defaultMaterial = CreateMaterial("default", "default");
        if (!m_defaultMaterial) {
            ErrorHandler::ThrowError("Failed to create default material", __FILE__, __func__, __LINE__);
        }

        MaterialSystem::SetVec3(m_defaultMaterial, "color", glm::vec3(1.0f));
        MaterialSystem::SetInt(m_defaultMaterial, "mainTexture", 0);
        MaterialSystem::SetInt(m_defaultMaterial, "useTexture", 1);

        m_defaultTexture = LoadTexture("default", "../assets/textures/default.png");
        if (!m_defaultTexture) {
            ErrorHandler::ThrowError("Failed to load default texture", __FILE__, __func__, __LINE__);
        }

        // 4. Default Cube Mesh
        const std::vector<Vertex> vertices = {
            // Front face
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
            {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // 1
            {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // 2
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // 3

            // Back face
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // 4
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // 5
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 6
            {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 7

            // Top face
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // 8
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // 9
            {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, // 10
            {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // 11

            // Bottom face
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // 12
            {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // 13
            {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // 14
            {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 15

            // Right face
            {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 16
            {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 17
            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 18
            {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 19

            // Left face
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 20
            {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 21
            {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 22
            {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}} // 23
        };

        const std::vector<uint32_t> indices = {
            // Front face
            0, 1, 2, 0, 2, 3,
            // Back face
            4, 5, 6, 4, 6, 7,
            // Top face
            8, 9, 10, 8, 10, 11,
            // Bottom face
            12, 13, 14, 12, 14, 15,
            // Right face
            16, 17, 18, 16, 18, 19,
            // Left face
            20, 21, 22, 20, 22, 23
        };

        m_defaultCubeMesh = LoadMesh("cube", vertices, indices);
        if (!m_defaultCubeMesh) {
            ErrorHandler::ThrowError("Failed to create default cube mesh", __FILE__, __func__, __LINE__);
        }
    }

    MeshSystem::Mesh *LoadMesh(const std::string &name, const std::vector<Vertex> &vertices,
                               const std::vector<uint32_t> &indices) {
        return MeshSystem::CreateMesh(name, vertices, indices);
    }

    MeshSystem::Mesh *GetMesh(const std::string &name) {
        return MeshSystem::GetMesh(name);
    }

    MeshSystem::Mesh *GetDefaultCubeMesh() {
        return m_defaultCubeMesh;
    }

    TextureSystem::Texture *LoadTexture(const std::string &name, const std::string &filePath) {
        return TextureSystem::CreateTexture(name, filePath);
    }

    TextureSystem::Texture *GetTexture(const std::string &name) {
        return TextureSystem::GetTexture(name);
    }

    TextureSystem::Texture *GetDefaultTexture() {
        return m_defaultTexture;
    }

    ShaderSystem::Shader *LoadShader(const std::string &name, const std::string &vertexPath,
                                     const std::string &fragmentPath) {
        return ShaderSystem::CreateShader(name, vertexPath, fragmentPath);
    }

    ShaderSystem::Shader *GetShader(const std::string &name) {
        return ShaderSystem::GetShader(name);
    }

    ShaderSystem::Shader *GetDefaultShader() {
        return m_defaultShader;
    }

    MaterialSystem::Material *CreateMaterial(const std::string &name, const std::string &shaderName) {
        return MaterialSystem::CreateMaterial(name, shaderName);
    }

    MaterialSystem::Material *CreateDefaultMaterial(const std::string &name) {
        return MaterialSystem::CreateDefaultMaterial(name);
    }

    MaterialSystem::Material *GetMaterial(const std::string &name) {
        return MaterialSystem::GetMaterial(name);
    }

    MaterialSystem::Material *GetDefaultMaterial() {
        return m_defaultMaterial;
    }

    void CleanUp() {
        TextureSystem::CleanUp();
        MeshSystem::CleanUp();
        MaterialSystem::CleanUp();
        ShaderSystem::CleanUp();

        m_defaultShader = nullptr;
        m_defaultMaterial = nullptr;
        m_defaultCubeMesh = nullptr;
        m_defaultTexture = nullptr;
    }
}
