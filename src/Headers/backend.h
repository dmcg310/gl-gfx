#pragma once

#include "common.h"

namespace Backend {
    void Init();

    void BeginFrame();

    void Update();

    void PrepareUi();

    void RenderUi();

    void EndFrame();

    void CleanUp();

    GLFWwindow *GetWindowHandle();

    void CreateGLFWWindow(const WindowedMode &windowedMode);

    void SetWindowedMode(const WindowedMode &windowedMode);

    void ToggleFullscreen();

    void ForceCloseWindow();

    bool WindowIsOpen();

    bool WindowIsMinimized();

    float GetWindowWidth();

    float GetWindowHeight();

    float GetWindowTime();

    int GetPlatform();
}
