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

    static std::unordered_map<size_t, BatchGroup> m_batchGroups;
    static auto m_clearColor = glm::vec4{0.05f, 0.05f, 0.1f, 1.0f};

    float m_lastFrameTime = 0.0f;
    constexpr uint32_t maxInstances = 2048;

    static size_t ComputeBatchHash(MeshSystem::Mesh *mesh, MaterialSystem::Material *material,
                                   TextureSystem::Texture *texture) {
        size_t hash = 0;
        hash = std::hash<void *>{}(mesh);
        hash ^= std::hash<void *>{}(material) << 1;
        hash ^= std::hash<void *>{}(texture) << 2;
        return hash;
    }

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

    void SubmitInstanced(MeshSystem::Mesh *mesh, MaterialSystem::Material *material, TextureSystem::Texture *texture,
                         const glm::mat4 &modelMatrix, const glm::vec4 &color) {
        if (!mesh || !material || !texture) {
            ErrorHandler::Warn("Error submitting instanced command to renderer. Mesh, transform or material not set",
                               __FILE__, __func__, __LINE__);
            return;
        }

        InstanceData instance{};
        instance.modelMatrix = modelMatrix;
        instance.color = color;

        const size_t batchHash = ComputeBatchHash(mesh, material, texture);
        m_batchGroups[batchHash].mesh = mesh;
        m_batchGroups[batchHash].material = material;
        m_batchGroups[batchHash].texture = texture;
        m_batchGroups[batchHash].instances.push_back(instance);
    }

    void SetMainCamera(CameraSystem::Camera *camera) {
        m_mainCamera = camera;
    }

    CameraSystem::Camera *GetMainCamera() {
        return m_mainCamera;
    }

    void SetClearColor(const glm::vec4 &color) {
        m_clearColor = color;
    }

    glm::vec4 GetClearColor() {
        return m_clearColor;
    }

    void Render() {
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

        glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!m_mainCamera) {
            return;
        }

        const glm::mat4 &viewMatrix = CameraSystem::GetViewMatrix(m_mainCamera);
        const glm::mat4 &projMatrix = CameraSystem::GetProjectionMatrix(m_mainCamera);

        for (auto &[_, batch]: m_batchGroups) {
            if (batch.instances.empty()) {
                continue;
            }

            size_t instancesProcessed = 0;
            while (instancesProcessed < batch.instances.size()) {
                const size_t currentBatchSize = std::min((size_t) maxInstances,
                                                         batch.instances.size() - instancesProcessed);

                std::vector<InstanceData> currentBatchInstances(
                    batch.instances.begin() + instancesProcessed,
                    batch.instances.begin() + instancesProcessed + currentBatchSize
                );

                if (!batch.mesh->isInstanced) {
                    MeshSystem::SetupInstancedMesh(batch.mesh, maxInstances);
                }

                MeshSystem::UpdateInstanceData(batch.mesh, currentBatchInstances);

                MaterialSystem::Bind(batch.material);
                MaterialSystem::SetMat4(batch.material, "view", viewMatrix);
                MaterialSystem::SetMat4(batch.material, "projection", projMatrix);
                MaterialSystem::SetInt(batch.material, "useInstanceColor", 1);

                if (batch.texture) {
                    TextureSystem::Bind(batch.texture, 0);
                }

                MeshSystem::Bind(batch.mesh);
                MeshSystem::DrawInstanced(batch.mesh);
                MeshSystem::Unbind();

                if (batch.texture) {
                    TextureSystem::Unbind();
                }

                MaterialSystem::Unbind();

                instancesProcessed += currentBatchSize;
            }
        }

        m_batchGroups.clear();
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

        m_defaultShader = nullptr;
        m_defaultMaterial = nullptr;
        m_cubeMesh = nullptr;
        m_defaultTexture = nullptr;
        m_mainCamera = nullptr;
    }
}
