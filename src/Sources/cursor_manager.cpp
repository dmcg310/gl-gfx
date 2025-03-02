#include "cursor_manager.h"

#include "backend.h"
#include "input.h"

namespace CursorManager {
    auto m_currentMode = CursorMode::CAMERA;
    bool m_wasInCameraMode = true;
    bool m_isLocked = false;

    void Init() {
        SetCursorMode(CursorMode::CAMERA);
    }

    void SetCursorMode(const CursorMode mode) {
        if (m_isLocked && mode != m_currentMode) {
            return;
        }

        const auto window = Backend::GetWindowHandle();
        if (!window) {
            return;
        }

        m_currentMode = mode;

        if (mode == CursorMode::CAMERA) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_wasInCameraMode = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_wasInCameraMode = false;
        }
    }

    void ToggleCursorMode() {
        if (m_isLocked) {
            return;
        }

        if (m_currentMode == CursorMode::CAMERA) {
            SetCursorMode(CursorMode::UI);
        } else {
            SetCursorMode(CursorMode::CAMERA);
        }
    }

    void LockCursorMode() {
        m_isLocked = true;
    }

    void UnlockCursorMode() {
        m_isLocked = false;
    }

    bool IsCursorModeLocked() {
        return m_isLocked;
    }

    CursorMode GetCursorMode() {
        return m_currentMode;
    }

    bool IsInCameraMode() {
        return m_currentMode == CursorMode::CAMERA;
    }

    bool IsInUiMode() {
        return m_currentMode == CursorMode::UI;
    }

    void CleanUp() {
        UnlockCursorMode();
        if (const auto window = Backend::GetWindowHandle()) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}
