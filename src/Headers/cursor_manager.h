#pragma once

#include "common.h"

namespace CursorManager {
    enum class CursorMode { CAMERA, UI };

    void Init();

    void SetCursorMode(CursorMode mode);

    void ToggleCursorMode();

    void LockCursorMode();

    void UnlockCursorMode();

    bool IsCursorModeLocked();

    CursorMode GetCursorMode();

    bool IsInCameraMode();

    bool IsInUiMode();

    void CleanUp();
}
