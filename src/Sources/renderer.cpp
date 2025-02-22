#include "renderer.h"

#include "material_system.h"
#include "mesh_system.h"
#include "shader_system.h"

#include <vector>

namespace Renderer {
    ShaderSystem::Shader *m_defaultShader = nullptr;
    MaterialSystem::Material *m_defaultMaterial = nullptr;
    MeshSystem::Mesh *m_triangleMesh = nullptr;

    void Init() {
        ShaderSystem::Init();
        MaterialSystem::Init();
        MeshSystem::Init();

        m_defaultShader = ShaderSystem::CreateShader(
            "default",
            "../src/Shaders/triangle.vert",
            "../src/Shaders/triangle.frag"
        );
        if (!m_defaultShader) {
            ErrorHandler::ThrowError("Failed to create default shader", __FILE__,
                                     __func__, __LINE__);
        }

        m_defaultMaterial = MaterialSystem::CreateMaterial("default", "default");
        if (!m_defaultMaterial) {
            ErrorHandler::ThrowError("Failed to create default material", __FILE__,
                                     __func__, __LINE__);
        }

        MaterialSystem::SetVec3(m_defaultMaterial, "color", glm::vec3(1.0f, 1.0f, 1.0f));

        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}
        };

        m_triangleMesh = MeshSystem::CreateMesh("triangle", vertices);
        if (!m_triangleMesh) {
            ErrorHandler::ThrowError("Failed to create mesh", __FILE__,
                                     __func__, __LINE__);
        }
    }

    void Render() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        MaterialSystem::Bind(m_defaultMaterial);

        MeshSystem::Bind(m_triangleMesh);
        MeshSystem::Draw(m_triangleMesh);
        MeshSystem::Unbind();

        MaterialSystem::Unbind();
    }

    void CleanUp() {
        MeshSystem::CleanUp();
        MaterialSystem::CleanUp();
        ShaderSystem::CleanUp();

        m_defaultShader = nullptr;
        m_defaultMaterial = nullptr;
        m_triangleMesh = nullptr;
    }
}
