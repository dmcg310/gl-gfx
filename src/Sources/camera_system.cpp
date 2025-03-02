#include "camera_system.h"

#include "backend.h"
#include "cursor_manager.h"
#include "input.h"

namespace CameraSystem {
    static std::unordered_map<std::string, Camera> m_cameras;
    static Camera *m_mainCamera = nullptr;

    void Init() {
        m_cameras.clear();
        m_mainCamera = nullptr;
    }

    Camera *CreateCamera(const std::string &name) {
        Camera camera{};
        camera.name = name;
        camera.transform = TransformSystem::CreateTransform(name + "_transform");

        m_cameras[name] = camera;

        if (!m_mainCamera) {
            m_mainCamera = &m_cameras[name];
        }

        return &m_cameras[name];
    }

    Camera *GetCamera(const std::string &name) {
        const auto it = m_cameras.find(name);
        return (it != m_cameras.end()) ? &it->second : nullptr;
    }

    static void UpdateCameraVectors(Camera *camera) {
        glm::vec3 front;
        front.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
        front.y = sin(glm::radians(camera->pitch));
        front.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));

        camera->front = glm::normalize(front);
        camera->right = glm::normalize(glm::cross(camera->front, camera->worldUp));
        camera->up = glm::normalize(glm::cross(camera->right, camera->front));
        camera->viewDirty = true;
    }

    void UpdateCamera(Camera *camera, const float deltaTime) {
        if (!camera || !camera->transform) {
            return;
        }

        if (!CursorManager::IsInCameraMode()) {
            return;
        }

        const float mouseX = Input::GetMouseOffsetX() * camera->mouseSensitivity;
        const float mouseY = Input::GetMouseOffsetY() * camera->mouseSensitivity;

        camera->yaw += mouseX;
        camera->pitch -= mouseY;
        camera->pitch = glm::clamp(camera->pitch, -89.0f, 89.0f);

        const float velocity = camera->moveSpeed * deltaTime;

        if (Input::KeyDown(KEY_W)) {
            camera->position += camera->front * velocity;
            camera->viewDirty = true;
        }
        if (Input::KeyDown(KEY_S)) {
            camera->position -= camera->front * velocity;
            camera->viewDirty = true;
        }
        if (Input::KeyDown(KEY_A)) {
            camera->position -= camera->right * velocity;
            camera->viewDirty = true;
        }
        if (Input::KeyDown(KEY_D)) {
            camera->position += camera->right * velocity;
            camera->viewDirty = true;
        }
        if (Input::KeyDown(KEY_Q)) {
            camera->position -= camera->up * velocity;
            camera->viewDirty = true;
        }
        if (Input::KeyDown(KEY_E)) {
            camera->position += camera->up * velocity;
            camera->viewDirty = true;
        }

        float zoomDelta = 0.0f;
        if (Input::MouseWheelUp()) {
            zoomDelta = -2.5f;
        }
        if (Input::MouseWheelDown()) {
            zoomDelta = 2.5f;
        }

        if (zoomDelta != 0.0f) {
            camera->zoom = glm::clamp(camera->zoom + zoomDelta, 1.0f, 90.0f);
            const float aspect = Backend::GetWindowWidth() / Backend::GetWindowHeight();
            camera->projMatrix =
                glm::perspective(glm::radians(camera->zoom), aspect, 0.1f, 100.0f);
        }

        TransformSystem::SetPosition(camera->transform, camera->position);

        UpdateCameraVectors(camera);
    }

    void SetProjection(Camera *camera, const float fov, const float aspect,
                       const float nearPlane, const float farPlane) {
        if (!camera) {
            return;
        }

        camera->fov = fov;
        camera->aspectRatio = aspect;
        camera->nearPlane = nearPlane;
        camera->farPlane = farPlane;
        camera->projMatrix =
            glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }

    const glm::mat4 &GetViewMatrix(Camera *camera) {
        if (!camera) {
            static glm::mat4 identityMatrix(1.0f);
            return identityMatrix;
        }

        if (camera->viewDirty) {
            camera->viewMatrix = glm::lookAt(
                camera->position, camera->position + camera->front, camera->up);
            camera->viewDirty = false;
        }

        return camera->viewMatrix;
    }

    const glm::mat4 &GetProjectionMatrix(const Camera *camera) {
        if (!camera) {
            static glm::mat4 identityMatrix(1.0f);
            return identityMatrix;
        }

        return camera->projMatrix;
    }

    TransformSystem::Transform *GetTransform(const Camera *camera) {
        return camera ? camera->transform : nullptr;
    }

    const glm::vec3 &GetPosition(const Camera *camera) {
        if (!camera) {
            static glm::vec3 defaultPosition(0.0f, 0.0f, 0.0f);
            return defaultPosition;
        }

        return camera->position;
    }

    void SetMainCamera(Camera *camera) {
        m_mainCamera = camera;
    }

    Camera *GetMainCamera() {
        return m_mainCamera;
    }

    void UpdateMainCameraProjection(const float windowWidth, const float windowHeight) {
        if (!m_mainCamera) {
            return;
        }

        float height = windowHeight;
        if (height <= 0) {
            height = 1.0f;
        }

        const float aspectRatio = windowWidth / height;
        SetProjection(m_mainCamera, m_mainCamera->fov, aspectRatio,
                      m_mainCamera->nearPlane, m_mainCamera->farPlane);
    }

    void CleanUp() {
        m_cameras.clear();
    }
}
