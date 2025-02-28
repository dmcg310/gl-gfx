#include "renderer.h"

#include "backend.h"
#include "camera_system.h"

#include <vector>

namespace Renderer {
    static CameraSystem::Camera *m_mainCamera = nullptr;
    static std::unordered_map<size_t, BatchGroup> m_batchGroups;
    static auto m_clearColor = glm::vec4(0.05f, 0.05f, 0.1f, 1.0f);
    static float m_lastFrameTime = 0.0f;
    static const uint32_t maxInstances = 2048;

    static size_t ComputeBatchHash(MeshSystem::Mesh *mesh, MaterialSystem::Material *material,
                                   TextureSystem::Texture *texture) {
        size_t hash = 0;
        hash = std::hash<void *>{}(mesh);
        hash ^= std::hash<void *>{}(material) << 1;
        hash ^= std::hash<void *>{}(texture) << 2;
        return hash;
    }

    void Init() {
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

    void CleanUp() {
        m_batchGroups.clear();
        m_mainCamera = nullptr;
        m_lastFrameTime = 0.0f;
    }
}
