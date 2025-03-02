#include "ui.h"

#include <renderer.h>

#include "backend.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "cursor_manager.h"
#include "imgui.h"
#include "input.h"
#include "scene_system.h"
#include "serialisation.h"

namespace Ui {
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
            float clearColor[4] = {currentClearColor.r, currentClearColor.g,
                                   currentClearColor.b, currentClearColor.a};

            if (ImGui::ColorEdit4("Background Color", clearColor)) {
                Renderer::SetClearColor(glm::vec4(clearColor[0], clearColor[1],
                                                  clearColor[2], clearColor[3]));
            }
        }
    }

    static void RenderEntityProperties(SceneSystem::Entity *entity, bool isLight) {
        if (entity->transform) {
            const glm::vec3 position = TransformSystem::GetPosition(entity->transform);
            if (float pos[3] = {position.x, position.y, position.z};
                ImGui::DragFloat3("Position", pos, 0.1f)) {
                TransformSystem::SetPosition(entity->transform,
                                             glm::vec3(pos[0], pos[1], pos[2]));

                if (isLight) {
                    for (const std::vector<LightSystem::Light *> &lights =
                             LightSystem::GetAllLights();
                         auto *light : lights) {
                        if (light->name == entity->name ||
                            light->name + "_visual" == entity->name) {
                            light->position = glm::vec3(pos[0], pos[1], pos[2]);
                            break;
                        }
                    }
                }
            }

            const glm::vec3 scale = TransformSystem::GetScale(entity->transform);
            if (float scl[3] = {scale.x, scale.y, scale.z};
                ImGui::DragFloat3("Scale", scl, 0.1f, 0.1f, 10.0f)) {
                TransformSystem::SetScale(entity->transform,
                                          glm::vec3(scl[0], scl[1], scl[2]));
            }

            const glm::vec3 rotation = entity->transform->rotation;
            if (float rot[3] = {rotation.x, rotation.y, rotation.z};
                ImGui::DragFloat3("Rotation", rot, 1.0f, -180.0f, 180.0f)) {
                TransformSystem::SetRotation(entity->transform,
                                             glm::vec3(rot[0], rot[1], rot[2]));
            }
        }

        float color[4] = {entity->color.r, entity->color.g, entity->color.b,
                          entity->color.a};

        if (ImGui::ColorEdit4("Color", color)) {
            entity->color = glm::vec4(color[0], color[1], color[2], color[3]);

            if (isLight) {
                for (const std::vector<LightSystem::Light *> &lights =
                         LightSystem::GetAllLights();
                     auto *light : lights) {
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
                 LightSystem::GetAllLights();
             auto *light : lights) {
            if (light->name == entityName || light->name + "_visual" == entityName) {
                float intensity = light->intensity;
                if (ImGui::SliderFloat("Light Intensity", &intensity, 0.0f, 5.0f)) {
                    light->intensity = intensity;
                }

                if (light->type == LightSystem::LightType::Directional) {
                    float direction[3] = {light->direction.x, light->direction.y,
                                          light->direction.z};

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
            const std::vector<SceneSystem::Entity *> &entities =
                SceneSystem::GetAllEntities();

            std::vector<const char *> entityNames;
            entityNames.reserve(entities.size());

            for (const auto *entity : entities) {
                entityNames.push_back(entity->name.c_str());
            }

            if (ImGui::Combo("Select Entity", &selectedEntityIndex, entityNames.data(),
                             static_cast<int>(entityNames.size()))) {
            }

            if (selectedEntityIndex >= 0 &&
                selectedEntityIndex < static_cast<int>(entities.size())) {
                SceneSystem::Entity *entity = entities[selectedEntityIndex];

                const bool isLight = entity->name.find("light") != std::string::npos ||
                                     entity->name.find("point") != std::string::npos;

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
        if (ImGui::CollapsingHeader("Material Properties",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
            const std::vector<SceneSystem::Entity *> &entities =
                SceneSystem::GetAllEntities();

            if (selectedEntityIndex >= 0 &&
                selectedEntityIndex < static_cast<int>(entities.size())) {
                SceneSystem::Entity *entity = entities[selectedEntityIndex];

                if (entity->material) {
                    bool updateMaterial = false;

                    float ambientStrength = MaterialSystem::GetFloat(
                        entity->material, "ambientStrength", 0.1f);
                    float diffuseStrength = MaterialSystem::GetFloat(
                        entity->material, "diffuseStrength", 0.7f);
                    float specularStrength = MaterialSystem::GetFloat(
                        entity->material, "specularStrength", 0.5f);
                    float shininess =
                        MaterialSystem::GetFloat(entity->material, "shininess");

                    updateMaterial |= ImGui::SliderFloat("Ambient Strength",
                                                         &ambientStrength, 0.0f, 1.0f);
                    updateMaterial |= ImGui::SliderFloat("Diffuse Strength",
                                                         &diffuseStrength, 0.0f, 1.0f);
                    updateMaterial |= ImGui::SliderFloat("Specular Strength",
                                                         &specularStrength, 0.0f, 1.0f);
                    updateMaterial |=
                        ImGui::SliderFloat("Shininess", &shininess, 0.0f, 10.0f);

                    int useTexture =
                        MaterialSystem::GetInt(entity->material, "useTexture", 0);
                    bool useTextureChecked = useTexture > 0;
                    if (ImGui::Checkbox("Use Texture", &useTextureChecked)) {
                        MaterialSystem::SetInt(entity->material, "useTexture",
                                               useTextureChecked ? 1 : 0);
                        updateMaterial = true;
                    }

                    int isEmissive =
                        MaterialSystem::GetInt(entity->material, "isEmissive", 0);
                    bool isEmissiveChecked = isEmissive > 0;
                    if (ImGui::Checkbox("Is Emissive", &isEmissiveChecked)) {
                        MaterialSystem::SetInt(entity->material, "isEmissive",
                                               isEmissiveChecked ? 1 : 0);
                        updateMaterial = true;
                    }

                    if (updateMaterial) {
                        MaterialSystem::SetFloat(entity->material, "ambientStrength",
                                                 ambientStrength);
                        MaterialSystem::SetFloat(entity->material, "diffuseStrength",
                                                 diffuseStrength);
                        MaterialSystem::SetFloat(entity->material, "specularStrength",
                                                 specularStrength);
                        MaterialSystem::SetFloat(entity->material, "shininess",
                                                 shininess);
                    }
                } else {
                    ImGui::Text("Selected entity has no material.");
                }
            } else {
                ImGui::Text(
                    "No entity selected. Select an entity to edit its material "
                    "properties.");
            }
        }
    }

    static void RenderSceneManagementPanel() {
        if (ImGui::CollapsingHeader("Scene Management", ImGuiTreeNodeFlags_DefaultOpen)) {
            const std::string currentSceneName = SceneSystem::GetSceneName();
            ImGui::Text("Current Scene: %s", currentSceneName.empty()
                                                 ? "Unnamed Scene"
                                                 : currentSceneName.c_str());

            ImGui::Separator();

            // saving
            if (ImGui::TreeNodeEx("Save Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
                static char sceneNameBuffer[128] = "";
                if (ImGui::Button("Use Current Name")) {
                    strncpy(sceneNameBuffer, currentSceneName.c_str(),
                            sizeof(sceneNameBuffer) - 1);
                    sceneNameBuffer[sizeof(sceneNameBuffer) - 1] = '\0';
                }

                ImGui::SameLine();
                ImGui::InputText("Scene Name", sceneNameBuffer,
                                 IM_ARRAYSIZE(sceneNameBuffer));
                static char filenameBuffer[128] = "";
                if (ImGui::Button("Use Scene Name")) {
                    strncpy(filenameBuffer, sceneNameBuffer, sizeof(filenameBuffer) - 1);
                    filenameBuffer[sizeof(filenameBuffer) - 1] = '\0';
                }

                ImGui::SameLine();
                ImGui::InputText("Filename", filenameBuffer,
                                 IM_ARRAYSIZE(filenameBuffer));
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("File will be saved to the scenes directory");
                }

                static bool showSaveConfirmation = false;
                if (ImGui::Button("Save Scene") && !showSaveConfirmation) {
                    if (strlen(sceneNameBuffer) > 0 && strlen(filenameBuffer) > 0) {
                        showSaveConfirmation = true;
                    } else {
                        ImGui::OpenPopup("Save Error");
                    }
                }

                if (ImGui::BeginPopupModal("Save Error", NULL,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Scene name and filename cannot be empty!");
                    ImGui::Separator();

                    if (ImGui::Button("OK", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                if (showSaveConfirmation) {
                    ImGui::OpenPopup("Save Scene?");
                }

                if (ImGui::BeginPopupModal("Save Scene?", NULL,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Are you sure you want to save the scene?");
                    ImGui::Text("Scene Name: %s", sceneNameBuffer);
                    ImGui::Text("Filename: %s", filenameBuffer);
                    ImGui::Separator();

                    if (ImGui::Button("Save", ImVec2(120, 0))) {
                        SceneSystem::SetSceneName(sceneNameBuffer);

                        if (Serialisation::Serialise(filenameBuffer, sceneNameBuffer)) {
                            ImGui::CloseCurrentPopup();
                            showSaveConfirmation = false;

                            sceneNameBuffer[0] = '\0';
                            filenameBuffer[0] = '\0';

                            ImGui::OpenPopup("Save Success");
                        } else {
                            ImGui::OpenPopup("Save Failed");
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                        showSaveConfirmation = false;
                    }

                    ImGui::EndPopup();
                }

                if (ImGui::BeginPopupModal("Save Success", NULL,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Scene saved successfully!");
                    ImGui::Separator();

                    if (ImGui::Button("OK", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                if (ImGui::BeginPopupModal("Save Failed", NULL,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Failed to save scene!");
                    ImGui::Separator();

                    if (ImGui::Button("OK", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                ImGui::TreePop();
            }

            ImGui::Separator();

            // loading
            if (ImGui::TreeNodeEx("Load Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
                static char loadFilenameBuffer[128] = "";
                ImGui::InputText("Filename to Load", loadFilenameBuffer,
                                 IM_ARRAYSIZE(loadFilenameBuffer));
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(
                        "Enter the scene file to load (from scenes directory)");
                }

                static bool showLoadConfirmation = false;
                if (ImGui::Button("Load Scene") && !showLoadConfirmation) {
                    if (strlen(loadFilenameBuffer) > 0) {
                        showLoadConfirmation = true;
                    } else {
                        ImGui::OpenPopup("Load Error");
                    }
                }

                if (ImGui::BeginPopupModal("Load Error", NULL,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Filename cannot be empty!");
                    ImGui::Separator();

                    if (ImGui::Button("OK", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                if (showLoadConfirmation) {
                    ImGui::OpenPopup("Load Scene?");
                }

                if (ImGui::BeginPopupModal("Load Scene?", NULL,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Are you sure you want to load the scene?");
                    ImGui::Text("All unsaved changes to the current scene will be lost.");
                    ImGui::Text("Filename: %s", loadFilenameBuffer);
                    ImGui::Separator();

                    if (ImGui::Button("Load", ImVec2(120, 0))) {
                        if (Serialisation::Deserialise(loadFilenameBuffer)) {
                            ImGui::CloseCurrentPopup();
                            showLoadConfirmation = false;
                            loadFilenameBuffer[0] = '\0';

                            ImGui::OpenPopup("Load Success");
                        } else {
                            ImGui::OpenPopup("Load Failed");
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                        showLoadConfirmation = false;
                    }

                    ImGui::EndPopup();
                }

                if (ImGui::BeginPopupModal("Load Success", NULL,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Scene loaded successfully!");
                    ImGui::Separator();

                    if (ImGui::Button("OK", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                if (ImGui::BeginPopupModal("Load Failed", NULL,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Failed to load scene!");
                    ImGui::Text("Check if the file exists in the scenes directory.");
                    ImGui::Separator();

                    if (ImGui::Button("OK", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                ImGui::TreePop();
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
        RenderSceneManagementPanel();

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

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(Backend::GetWindowHandle(), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void BeginFrame() {
        // we have to capture tab before imgui does for ui navigation
        static bool tabWasPressed = false;

        bool tabIsPressed = Input::KeyPressed(KEY_TAB);
        if (tabIsPressed && !tabWasPressed) {
            const bool wasLocked = CursorManager::IsCursorModeLocked();
            if (wasLocked) {
                CursorManager::UnlockCursorMode();
            }

            CursorManager::ToggleCursorMode();
            if (wasLocked) {
                CursorManager::LockCursorMode();
            }
        }

        tabWasPressed = tabIsPressed;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiIO &io = ImGui::GetIO();
// apparently we are outside the bounds of an array here but the code
// seems to work
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warray-bounds"
        // don't handle tab
        io.KeysData[ImGuiKey_Tab].Down = false;
        io.KeysData[ImGuiKey_Tab].DownDuration = -1.0f;
        io.KeysData[ImGuiKey_Tab].DownDurationPrev = -1.0f;
#pragma clang diagnostic pop

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
    }

    void CleanUp() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}
