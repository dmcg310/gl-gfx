#include "input.h"

#include "backend.h"

namespace Input {
    bool m_keyPressed[372];
    bool m_keyDown[372];
    bool m_keyDownLastFrame[372];
    double m_mouseX = 0;
    double m_mouseY = 0;
    double m_mouseOffsetX = 0;
    double m_mouseOffsetY = 0;
    int m_mouseWheelValue = 0;
    int m_sensitivity = 100;
    bool m_mouseWheelUp = false;
    bool m_mouseWheelDown = false;
    bool m_leftMouseDown = false;
    bool m_rightMouseDown = false;
    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;
    bool m_leftMouseDownLastFrame = false;
    bool m_rightMouseDownLastFrame = false;
    bool m_preventRightMouseHoldTillNextClick = false;
    int m_mouseScreenX = 0;
    int m_mouseScreenY = 0;
    int m_scrollWheelYOffset = 0;

    GLFWwindow *m_window;

    void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

    void Init() {
        double x, y;

        m_window = Backend::GetWindowHandle();

        glfwSetScrollCallback(m_window, scroll_callback);
        glfwGetCursorPos(m_window, &x, &y);
        DisableCursor();

        m_mouseOffsetX = x;
        m_mouseOffsetY = y;
        m_mouseX = x;
        m_mouseY = y;
    }

    void Update() {
        if (KeyPressed(KEY_ESCAPE)) {
            Backend::ForceCloseWindow();
        }

        if (KeyPressed(KEY_F)) {
            Backend::ToggleFullscreen();
        }

        m_mouseWheelUp = false;
        m_mouseWheelDown = false;
        m_mouseWheelValue = GetScrollWheelYOffset();

        if (m_mouseWheelValue < 0) {
            m_mouseWheelDown = true;
        }

        if (m_mouseWheelValue > 0) {
            m_mouseWheelUp = true;
        }

        ResetScrollWheelYOffset();

        for (int i = 32; i < 349; i++) {
            if (glfwGetKey(m_window, i) == GLFW_PRESS) {
                m_keyDown[i] = true;
            } else {
                m_keyDown[i] = false;
            }

            if (m_keyDown[i] && !m_keyDownLastFrame[i]) {
                m_keyPressed[i] = true;
            } else {
                m_keyPressed[i] = false;
            }

            m_keyDownLastFrame[i] = m_keyDown[i];
        }

        double x, y;
        glfwGetCursorPos(m_window, &x, &y);

        m_mouseOffsetX = x - m_mouseX;
        m_mouseOffsetY = y - m_mouseY;
        m_mouseX = x;
        m_mouseY = y;

        m_leftMouseDown = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
        if (m_leftMouseDown == GLFW_PRESS && !m_leftMouseDownLastFrame) {
            m_leftMousePressed = true;
        } else {
            m_leftMousePressed = false;
        }
        m_leftMouseDownLastFrame = m_leftMouseDown;

        m_rightMouseDown = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT);
        if (m_rightMouseDown == GLFW_PRESS && !m_rightMouseDownLastFrame) {
            m_rightMousePressed = true;
        } else {
            m_rightMousePressed = false;
        }
        m_rightMouseDownLastFrame = m_rightMouseDown;

        if (m_rightMousePressed) {
            m_preventRightMouseHoldTillNextClick = false;
        }
    }

    bool KeyPressed(unsigned int keycode) {
        return m_keyPressed[keycode];
    }

    bool KeyDown(unsigned int keycode) {
        return m_keyDown[keycode];
    }

    float GetMouseOffsetX() {
        return (float)m_mouseOffsetX;
    }

    float GetMouseOffsetY() {
        return (float)m_mouseOffsetY;
    }

    bool LeftMouseDown() {
        return m_leftMouseDown;
    }

    bool RightMouseDown() {
        return m_rightMouseDown && !m_preventRightMouseHoldTillNextClick;
    }

    bool LeftMousePressed() {
        return m_leftMousePressed;
    }

    bool RightMousePressed() {
        return m_rightMousePressed;
    }

    bool MouseWheelUp() {
        return m_mouseWheelUp;
    }

    bool MouseWheelDown() {
        return m_mouseWheelDown;
    }

    int GetMouseWheelValue() {
        return m_mouseWheelValue;
    }

    int GetMouseX() {
        return (int)m_mouseX;
    }

    int GetMouseY() {
        return (int)m_mouseY;
    }

    void PreventRightMouseHold() {
        m_preventRightMouseHoldTillNextClick = true;
    }

    int GetScrollWheelYOffset() {
        return m_scrollWheelYOffset;
    }

    void ResetScrollWheelYOffset() {
        m_scrollWheelYOffset = 0;
    }

    void DisableCursor() {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void HideCursor() {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void ShowCursor() {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    int GetCursorScreenX() {
        return m_mouseScreenX;
    }

    int GetCursorScreenY() {
        return m_mouseScreenY;
    }

    int GetViewportMappedMouseX(int /*viewportWidth*/) {
        return 0;
    }

    int GetViewportMappedMouseY(int /*viewportHeight*/) {
        return 0;
    }

    void scroll_callback(GLFWwindow * /*window*/, double /*xOffset*/, double yOffset) {
        m_scrollWheelYOffset = (int)yOffset;
    }
}
