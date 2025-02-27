#include "renderer.h"

#include "backend.h"
#include "shader_system.h"

#include <vector>

namespace Renderer {
    ShaderSystem::Shader *m_defaultShader = nullptr;
    MaterialSystem::Material *m_defaultMaterial = nullptr;
    MeshSystem::Mesh *m_cubeMesh = nullptr;
    TextureSystem::Texture *m_defaultTexture = nullptr;
    CameraSystem::Camera *m_mainCamera = nullptr;

    std::vector<RenderCommand> m_renderQueue;

    float m_lastFrameTime = 0.0f;

    void Init() {
        ShaderSystem::Init();
        MaterialSystem::Init();
        MeshSystem::Init();
        TextureSystem::Init();
        TransformSystem::Init();
        CameraSystem::Init();

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

        MaterialSystem::SetVec3(m_defaultMaterial, "color", glm::vec3(1.0f));
        MaterialSystem::SetInt(m_defaultMaterial, "mainTexture", 0);
        MaterialSystem::SetInt(m_defaultMaterial, "useTexture", 1);

        m_defaultTexture = TextureSystem::CreateTexture("default", "../assets/textures/default.png");
        if (!m_defaultTexture) {
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

        m_mainCamera = CameraSystem::CreateCamera("main");
        if (!m_mainCamera) {
            ErrorHandler::ThrowError("Failed to create camera", __FILE__, __func__, __LINE__);
        }

        UpdateProjection();
    }

    void UpdateProjection() {
        if (!m_mainCamera) {
            return;
        }

        const float width = Backend::GetWindowWidth();
        float height = Backend::GetWindowHeight();

        if (height == 0) {
            height = 1;
        }

        const float aspectRatio = width / height;
        CameraSystem::SetProjection(m_mainCamera, 45.0f, aspectRatio, 0.1f, 100.f);
    }

    void Submit(MeshSystem::Mesh *mesh, MaterialSystem::Material *material, TextureSystem::Texture *texture,
                TransformSystem::Transform *transform) {
        if (!mesh || !material || !transform) {
            ErrorHandler::Warn("Error submitting command to renderer. Mesh, transform or material not set", __FILE__,
                               __func__, __LINE__);
            return;
        }

        RenderCommand cmd{};
        cmd.mesh = mesh;
        cmd.material = material;
        cmd.texture = texture;
        cmd.modelMatrix = TransformSystem::GetModelMatrix(transform);

        m_renderQueue.push_back(cmd);
    }

    void ClearQueue() {
        m_renderQueue.clear();
    }

    void SetMainCamera(CameraSystem::Camera *camera) {
        m_mainCamera = camera;
    }

    CameraSystem::Camera *GetMainCamera() {
        return m_mainCamera;
    }

    void Render() {
        // TODO: fix, this is wrong
        const float currentFrame = Backend::GetWindowTime();
        const float deltaTime = currentFrame - m_lastFrameTime;
        m_lastFrameTime = deltaTime;

        if (m_mainCamera) {
            CameraSystem::UpdateCamera(m_mainCamera, deltaTime);
        }

        static float lastWidth = Backend::GetWindowWidth();
        static float lastHeight = Backend::GetWindowHeight();
        const float currentWidth = Backend::GetWindowWidth();
        const float currentHeight = Backend::GetWindowHeight();

        if (currentWidth != lastWidth || currentHeight != lastHeight) {
            UpdateProjection();
            lastWidth = currentWidth;
            lastHeight = currentHeight;
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!m_mainCamera) {
            return;
        }

        const glm::mat4 &viewMatrix = CameraSystem::GetViewMatrix(m_mainCamera);
        const glm::mat4 &projMatrix = CameraSystem::GetProjectionMatrix(m_mainCamera);

        std::sort(m_renderQueue.begin(), m_renderQueue.end());

        MaterialSystem::Material *currentMaterial = nullptr;
        TextureSystem::Texture *currentTexture = nullptr;
        MeshSystem::Mesh *currentMesh = nullptr;

        for (const auto &[mesh, material, texture, modelMatrix]: m_renderQueue) {
            if (material != currentMaterial) {
                if (currentMaterial) {
                    MaterialSystem::Unbind();
                }

                MaterialSystem::SetMat4(material, "view", viewMatrix);
                MaterialSystem::SetMat4(material, "projection", projMatrix);
                MaterialSystem::Bind(material);

                currentMaterial = material;
            }

            if (texture != currentTexture) {
                if (currentTexture) {
                    TextureSystem::Unbind();
                }

                if (texture) {
                    TextureSystem::Bind(texture);
                }

                currentTexture = texture;
            }

            if (mesh != currentMesh) {
                if (currentMesh) {
                    MeshSystem::Unbind();
                }

                MeshSystem::Bind(mesh);

                currentMesh = mesh;
            }

            MaterialSystem::SetMat4(currentMaterial, "model", modelMatrix);

            MeshSystem::Draw(mesh);
        }

        if (currentMesh) {
            MeshSystem::Unbind();
        }

        if (currentMaterial) {
            MaterialSystem::Unbind();
        }

        if (currentTexture) {
            TextureSystem::Unbind();
        }
    }

    MeshSystem::Mesh *GetDefaultCubeMesh() {
        return m_cubeMesh;
    }

    MaterialSystem::Material *GetDefaultMaterial() {
        return m_defaultMaterial;
    }

    TextureSystem::Texture *GetDefaultTexture() {
        return m_defaultTexture;
    }

    void CleanUp() {
        CameraSystem::CleanUp();
        TransformSystem::CleanUp();
        TextureSystem::CleanUp();
        MeshSystem::CleanUp();
        MaterialSystem::CleanUp();
        ShaderSystem::CleanUp();

        m_renderQueue.clear();
        m_defaultShader = nullptr;
        m_defaultMaterial = nullptr;
        m_cubeMesh = nullptr;
        m_defaultTexture = nullptr;
        m_mainCamera = nullptr;
    }
}
