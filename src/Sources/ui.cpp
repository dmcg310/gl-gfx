#include "ui.h"

#include <renderer.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "backend.h"
#include "cursor_manager.h"

namespace Ui {
    static void CreateMainPanel() {
        static bool firstTime = true;
        static ImVec2 windowPos(20, 20);

        if (firstTime) {
            ImGui::SetNextWindowPos(windowPos);
            firstTime = false;
        }

        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Frametime: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        }

        if (ImGui::CollapsingHeader("Visual Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            const glm::vec4 currentClearColor = Renderer::GetClearColor();
            float clearColor[4] = {
                currentClearColor.r,
                currentClearColor.g,
                currentClearColor.b,
                currentClearColor.a
            };

            if (ImGui::ColorEdit4("Background Color", clearColor)) {
                Renderer::SetClearColor(glm::vec4(
                    clearColor[0],
                    clearColor[1],
                    clearColor[2],
                    clearColor[3]
                ));
            }
        }

        if (ImGui::IsWindowAppearing()) {
            windowPos = ImGui::GetWindowPos();
        }

        ImGui::End();
    }

    void Init() {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGuiStyle &style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplGlfw_InitForOpenGL(Backend::GetWindowHandle(), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }


    void BeginFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiIO &io = ImGui::GetIO();
        if (CursorManager::IsInCameraMode()) {
            static ImVec2 lastMousePos = io.MousePos;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
            io.MousePos = lastMousePos;
        } else {
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

            if (!CursorManager::IsCursorModeLocked()) {
                if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
                    if (CursorManager::IsInCameraMode()) {
                        CursorManager::SetCursorMode(CursorManager::CursorMode::UI);
                    }
                }
            }
        }
    }

    void Render() {
        CreateMainPanel();
    }

    void EndFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (const ImGuiIO &io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void CleanUp() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}
