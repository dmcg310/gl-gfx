#include "ui.h"

#include <renderer.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "backend.h"
#include "cursor_manager.h"
#include "scene_system.h"
#include "resource_manager.h"

namespace Ui {
    static float ambientStrength = 0.1f;
    static float diffuseStrength = 0.7f;
    static float specularStrength = 0.5f;
    static float shininess = 32.0f;
    static int selectedEntityIndex = -1;
    static bool firstTime = true;
    static ImVec2 windowPos(20, 20);

    static void RenderPerformanceSection() {
        if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Frametime: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        }
    }

    static void RenderVisualSettingsSection() {
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
    }

    static void RenderEntityProperties(SceneSystem::Entity *entity, bool isLight) {
        if (entity->transform) {
            const glm::vec3 position = TransformSystem::GetPosition(entity->transform);
            if (float pos[3] = {position.x, position.y, position.z}; ImGui::DragFloat3("Position", pos, 0.1f)) {
                TransformSystem::SetPosition(entity->transform, glm::vec3(pos[0], pos[1], pos[2]));

                if (isLight) {
                    for (const std::vector<LightSystem::Light *> &lights =
                                 LightSystem::GetAllLights(); auto *light: lights) {
                        if (light->name == entity->name ||
                            light->name + "_visual" == entity->name) {
                            light->position = glm::vec3(pos[0], pos[1], pos[2]);
                            break;
                        }
                    }
                }
            }

            const glm::vec3 scale = TransformSystem::GetScale(entity->transform);
            if (float scl[3] = {scale.x, scale.y, scale.z}; ImGui::DragFloat3("Scale", scl, 0.1f, 0.1f, 10.0f)) {
                TransformSystem::SetScale(entity->transform, glm::vec3(scl[0], scl[1], scl[2]));
            }

            const glm::vec3 rotation = entity->transform->rotation;
            if (float rot[3] = {rotation.x, rotation.y, rotation.z};
                ImGui::DragFloat3("Rotation", rot, 1.0f, -180.0f, 180.0f)) {
                TransformSystem::SetRotation(entity->transform, glm::vec3(rot[0], rot[1], rot[2]));
            }
        }

        float color[4] = {
            entity->color.r,
            entity->color.g,
            entity->color.b,
            entity->color.a
        };

        if (ImGui::ColorEdit4("Color", color)) {
            entity->color = glm::vec4(color[0], color[1], color[2], color[3]);

            if (isLight) {
                for (const std::vector<LightSystem::Light *> &lights =
                             LightSystem::GetAllLights(); auto *light: lights) {
                    if (light->name == entity->name ||
                        light->name + "_visual" == entity->name) {
                        light->color = glm::vec3(color[0], color[1], color[2]);
                        break;
                    }
                }
            }
        }
    }

    static void RenderLightProperties(const std::string &entityName) {
        for (const std::vector<LightSystem::Light *> &lights =
                     LightSystem::GetAllLights(); auto *light: lights) {
            if (light->name == entityName || light->name + "_visual" == entityName) {
                float intensity = light->intensity;
                if (ImGui::SliderFloat("Light Intensity", &intensity, 0.0f, 5.0f)) {
                    light->intensity = intensity;
                }

                if (light->type == LightSystem::LightType::Directional) {
                    float direction[3] = {
                        light->direction.x,
                        light->direction.y,
                        light->direction.z
                    };

                    if (ImGui::DragFloat3("Direction", direction, 0.1f, -1.0f, 1.0f)) {
                        const float length = sqrtf(direction[0] * direction[0] +
                                                   direction[1] * direction[1] +
                                                   direction[2] * direction[2]);
                        if (length > 0.0001f) {
                            light->direction.x = direction[0] / length;
                            light->direction.y = direction[1] / length;
                            light->direction.z = direction[2] / length;
                        }
                    }
                }

                break;
            }
        }
    }

    static void RenderEntityControlsSection() {
        if (ImGui::CollapsingHeader("Entity Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
            const std::vector<SceneSystem::Entity *> &entities = SceneSystem::GetAllEntities();

            std::vector<const char *> entityNames;
            entityNames.reserve(entities.size());

            for (const auto *entity: entities) {
                entityNames.push_back(entity->name.c_str());
            }

            if (ImGui::Combo("Select Entity", &selectedEntityIndex,
                             entityNames.data(), static_cast<int>(entityNames.size()))) {
            }

            if (selectedEntityIndex >= 0 && selectedEntityIndex < entities.size()) {
                SceneSystem::Entity *entity = entities[selectedEntityIndex];

                const bool isLight = entity->name.find("light") != std::string::npos
                                     || entity->name.find("point") != std::string::npos;

                ImGui::Separator();
                ImGui::Text("Entity: %s", entity->name.c_str());

                RenderEntityProperties(entity, isLight);

                if (isLight) {
                    RenderLightProperties(entity->name);
                }
            }
        }
    }

    static void RenderMaterialPropertiesSection() {
        if (ImGui::CollapsingHeader("Material Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
            bool updateMaterial = false;

            updateMaterial |= ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f);
            updateMaterial |= ImGui::SliderFloat("Diffuse Strength", &diffuseStrength, 0.0f, 1.0f);
            updateMaterial |= ImGui::SliderFloat("Specular Strength", &specularStrength, 0.0f, 1.0f);
            updateMaterial |= ImGui::SliderFloat("Shininess", &shininess, 1.0f, 256.0f);

            if (updateMaterial) {
                if (MaterialSystem::Material *defaultMaterial = ResourceManager::GetDefaultMaterial()) {
                    MaterialSystem::SetFloat(defaultMaterial, "ambientStrength", ambientStrength);
                    MaterialSystem::SetFloat(defaultMaterial, "diffuseStrength", diffuseStrength);
                    MaterialSystem::SetFloat(defaultMaterial, "specularStrength", specularStrength);
                    MaterialSystem::SetFloat(defaultMaterial, "shininess", shininess);
                }
            }
        }
    }

    static void RenderMainPanel() {
        if (firstTime) {
            ImGui::SetNextWindowPos(windowPos);
            firstTime = false;
        }

        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        RenderPerformanceSection();
        RenderVisualSettingsSection();
        RenderEntityControlsSection();
        RenderMaterialPropertiesSection();

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
        RenderMainPanel();
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
