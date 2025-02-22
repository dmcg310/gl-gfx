#include "renderer.h"

#include "shader_system.h"
#include "material_system.h"
#include "mesh_system.h"
#include "texture_system.h"
#include "transform_system.h"

#include <vector>

namespace Renderer {
    ShaderSystem::Shader *m_defaultShader = nullptr;
    MaterialSystem::Material *m_defaultMaterial = nullptr;
    MeshSystem::Mesh *m_cubeMesh = nullptr;
    TextureSystem::Texture *m_texture = nullptr;
    TransformSystem::Transform *m_cubeTransform = nullptr;

    glm::vec3 m_rotation{0.0f};

    void Init() {
        ShaderSystem::Init();
        MaterialSystem::Init();
        MeshSystem::Init();
        TextureSystem::Init();
        TransformSystem::Init();

        m_defaultShader = ShaderSystem::CreateShader(
            "default",
            "../src/Shaders/default.vert",
            "../src/Shaders/default.frag"
        );
        if (!m_defaultShader) {
            ErrorHandler::ThrowError("Failed to create default shader", __FILE__, __func__, __LINE__);
        }

        m_defaultMaterial = MaterialSystem::CreateMaterial("default", "default");
        if (!m_defaultMaterial) {
            ErrorHandler::ThrowError("Failed to create default material", __FILE__, __func__, __LINE__);
        }

        m_texture = TextureSystem::CreateTexture("default", "../assets/textures/default.png");
        if (!m_texture) {
            ErrorHandler::ThrowError("Failed to load texture", __FILE__, __func__, __LINE__);
        }

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

        m_cubeMesh = MeshSystem::CreateMesh("cube", vertices, indices);
        if (!m_cubeMesh) {
            ErrorHandler::ThrowError("Failed to create mesh", __FILE__, __func__, __LINE__);
        }

        m_cubeTransform = TransformSystem::CreateTransform("cube");
        if (!m_cubeTransform) {
            ErrorHandler::ThrowError("Failed to create quad transform", __FILE__, __func__, __LINE__);
        }

        TransformSystem::SetPosition(m_cubeTransform, glm::vec3(0.0f, 0.0f, -2.0f));

        MaterialSystem::SetVec3(m_defaultMaterial, "color", glm::vec3(1.0f));
        MaterialSystem::SetInt(m_defaultMaterial, "mainTexture", 0);
        MaterialSystem::SetInt(m_defaultMaterial, "useTexture", 1);

        constexpr float aspectRatio = 800.0f / 600.0f;
        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
        MaterialSystem::SetMat4(m_defaultMaterial, "projection", projection);
    }

    void Render() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_rotation.x += 0.0125f;
        m_rotation.y += 0.025f;
        TransformSystem::SetRotation(m_cubeTransform, m_rotation);

        MaterialSystem::SetMat4(m_defaultMaterial, "model", TransformSystem::GetModelMatrix(m_cubeTransform));

        TextureSystem::Bind(m_texture, 0);

        MaterialSystem::Bind(m_defaultMaterial);

        MeshSystem::Bind(m_cubeMesh);
        MeshSystem::Draw(m_cubeMesh);
        MeshSystem::Unbind();

        MaterialSystem::Unbind();
        TextureSystem::Unbind(0);
    }

    void CleanUp() {
        TransformSystem::CleanUp();
        TextureSystem::CleanUp();
        MeshSystem::CleanUp();
        MaterialSystem::CleanUp();
        ShaderSystem::CleanUp();

        m_defaultShader = nullptr;
        m_defaultMaterial = nullptr;
        m_cubeMesh = nullptr;
    }
}
