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
    MeshSystem::Mesh *m_quadMesh = nullptr;
    TextureSystem::Texture *m_texture = nullptr;
    TransformSystem::Transform *m_quadTransform = nullptr;

    float m_rotationAngle = 0.0f;

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
            // Bottom left
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            // Bottom right
            {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            // Top right
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            // Top left
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
        };

        const std::vector<uint32_t> indices = {
            0, 1, 2,
            0, 2, 3
        };

        m_quadMesh = MeshSystem::CreateMesh("quad", vertices, indices);
        if (!m_quadMesh) {
            ErrorHandler::ThrowError("Failed to create mesh", __FILE__, __func__, __LINE__);
        }

        m_quadTransform = TransformSystem::CreateTransform("quad");
        if (!m_quadTransform) {
            ErrorHandler::ThrowError("Failed to create quad transform", __FILE__, __func__, __LINE__);
        }

        MaterialSystem::SetVec3(m_defaultMaterial, "color", glm::vec3(1.0f));
        MaterialSystem::SetInt(m_defaultMaterial, "mainTexture", 0);
        MaterialSystem::SetInt(m_defaultMaterial, "useTexture", 1);

        TransformSystem::SetPosition(m_quadTransform, glm::vec3(0.0f));
        TransformSystem::SetScale(m_quadTransform, glm::vec3(1.0f));
    }

    void Render() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_rotationAngle += 0.01f;
        TransformSystem::SetRotation(m_quadTransform, glm::vec3(0.0f, 0.0f, m_rotationAngle));

        MaterialSystem::SetMat4(m_defaultMaterial, "model", TransformSystem::GetModelMatrix(m_quadTransform));

        TextureSystem::Bind(m_texture, 0);

        MaterialSystem::Bind(m_defaultMaterial);

        MeshSystem::Bind(m_quadMesh);
        MeshSystem::Draw(m_quadMesh);
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
        m_quadMesh = nullptr;
    }
}
