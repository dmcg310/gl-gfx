#include "serialisation.h"

#include "camera_system.h"
#include "light_system.h"
#include "render_system.h"
#include "resource_manager.h"
#include "scene_system.h"
#include "transform_system.h"

namespace Serialisation {
    bool Serialise(const std::string &filename, const std::string &sceneName) {
        toml::table scene;
        scene.insert("scene_name", sceneName);

        toml::table camera;
        if (const auto *mainCamera = CameraSystem::GetMainCamera()) {
            camera.insert("name", mainCamera->name);
            camera.insert("position", ToTomlArray(mainCamera->position));
            camera.insert("up", ToTomlArray(mainCamera->up));
        }
        scene.insert("camera", camera);

        toml::array entities;
        for (const auto *entity : SceneSystem::GetAllEntities()) {
            toml::table entityTable;
            entityTable.insert("name", entity->name);
            entityTable.insert("color", ToTomlArray(entity->color));

            toml::table transform;
            glm::vec3 position = TransformSystem::GetPosition(entity->transform);
            glm::vec3 scale = TransformSystem::GetScale(entity->transform);
            glm::vec3 rotation = TransformSystem::GetRotation(entity->transform);

            transform.insert("position", ToTomlArray(position));
            transform.insert("scale", ToTomlArray(scale));
            transform.insert("rotation", ToTomlArray(rotation));

            entityTable.insert("transform", transform);

            if (entity->mesh) {
                toml::table mesh;
                mesh.insert("name", entity->mesh->name);
                mesh.insert("path", entity->mesh->path);

                entityTable.insert("mesh", mesh);
            }

            if (entity->texture) {
                toml::table texture;
                texture.insert("name", entity->texture->name);
                texture.insert("path", entity->texture->path);

                entityTable.insert("texture", texture);
            }

            if (entity->material) {
                toml::table material;
                material.insert("name", entity->material->name);

                if (entity->material->shader) {
                    toml::table shader;
                    shader.insert("name", entity->material->shader->name);
                    shader.insert("fragment_path", entity->material->shader->fragPath);
                    shader.insert("vertex_path", entity->material->shader->vertPath);

                    material.insert("shader", shader);
                }

                toml::table properties;
                for (const auto &[name, prop] : entity->material->properties) {
                    if (!prop.persistent) {
                        continue;
                    }

                    switch (prop.type) {
                        case MaterialSystem::Property::Type::Float:
                            properties.insert(name, std::any_cast<float>(prop.value));
                            break;
                        case MaterialSystem::Property::Type::Int:
                            properties.insert(name, std::any_cast<int>(prop.value));
                            break;
                        case MaterialSystem::Property::Type::Vec2: {
                            auto vec = std::any_cast<glm::vec2>(prop.value);
                            properties.insert(name, ToTomlArray(vec));
                            break;
                        }
                        case MaterialSystem::Property::Type::Vec3: {
                            auto vec = std::any_cast<glm::vec3>(prop.value);
                            properties.insert(name, ToTomlArray(vec));
                            break;
                        }
                        case MaterialSystem::Property::Type::Vec4: {
                            auto vec = std::any_cast<glm::vec4>(prop.value);
                            properties.insert(name, ToTomlArray(vec));
                            break;
                        }
                        case MaterialSystem::Property::Type::Mat4: {
                            auto mat = std::any_cast<glm::mat4>(prop.value);
                            properties.insert(name, ToTomlArray(mat));
                            break;
                        }
                        default:
                            ErrorHandler::Warn("Unknown property type", __FILE__,
                                               __func__, __LINE__);
                    }
                }

                material.insert("properties", properties);

                entityTable.insert("material", material);
            }

            entities.push_back(entityTable);
        }
        scene.insert("entity", entities);

        toml::array lights;
        for (const auto *light : LightSystem::GetAllLights()) {
            toml::table lightTable;
            lightTable.insert("name", light->name);
            lightTable.insert("type", light->type == LightSystem::LightType::Directional
                                          ? "directional"
                                          : "point");
            lightTable.insert("position", ToTomlArray(light->position));
            if (light->type == LightSystem::LightType::Directional) {
                lightTable.insert("directional", ToTomlArray(light->direction));
            }
            lightTable.insert("color", ToTomlArray(light->color));
            lightTable.insert("intensity", light->intensity);

            lights.push_back(lightTable);
        }
        scene.insert("light", lights);

        return Write(scene, filename);
    }

    bool Deserialise(const std::string &filename) {
        auto scene = Read(filename);
        if (scene.empty()) {
            ErrorHandler::Warn("Failed to load scene: " + filename, __FILE__, __func__,
                               __LINE__);
            return false;
        }

        SceneSystem::CleanUp();

        if (scene.contains("scene_name") && scene["scene_name"].is_string()) {
            const std::string sceneName =
                scene["scene_name"].as_string()->value_or("Unnamed Scene");
            SceneSystem::SetSceneName(sceneName);
        }

        DeserialiseLights(scene);

        return (DeserialiseCamera(scene) && DeserialiseEntities(scene));
    }

    bool DeserialiseCamera(const toml::table &scene) {
        if (!scene.contains("camera") || !scene["camera"].is_table()) {
            return false;
        }

        auto &cameraTable = *scene["camera"].as_table();
        const std::string cameraName = cameraTable["name"].as_string()->get();

        auto *camera = CameraSystem::CreateCamera(cameraName);
        CameraSystem::SetMainCamera(camera);

        RenderSystem::UpdateProjection();

        camera->position = ToVec3(*cameraTable["position"].as_array());
        camera->up = ToVec3(*cameraTable["up"].as_array());

        return true;
    }

    bool DeserialiseEntities(const toml::table &scene) {
        if (!scene.contains("entity") || !scene["entity"].is_array()) {
            return false;
        }

        for (const auto &entities = *scene["entity"].as_array();
             auto &entityValue : entities) {
            auto &entityTable = *entityValue.as_table();

            std::string name = entityTable["name"].as_string()->get();
            glm::vec4 color = ToVec4(*entityTable["color"].as_array());

            const auto *entity = SceneSystem::CreateEntity(name, color);

            if (entityTable.contains("transform")) {
                DeserialiseTransform(entity, *entityTable["transform"].as_table());
            }

            if (entityTable.contains("mesh")) {
                DeserialiseMesh(entity, *entityTable["mesh"].as_table());
            }

            if (entityTable.contains("texture")) {
                DeserialiseTexture(entity, *entityTable["texture"].as_table());
            }

            if (entityTable.contains("material")) {
                DeserialiseMaterial(entity, *entityTable["material"].as_table());
            }
        }

        return true;
    }

    void DeserialiseLights(const toml::table &scene) {
        if (!scene.contains("light") || !scene["light"].is_array()) {
            return;
        }

        for (const auto &lights = *scene["light"].as_array(); auto &lightValue : lights) {
            auto &lightTable = *lightValue.as_table();

            std::string name = lightTable["name"].as_string()->get();
            std::string typeStr = lightTable["type"].as_string()->get();
            const LightSystem::LightType type = (typeStr == "directional")
                                                    ? LightSystem::LightType::Directional
                                                    : LightSystem::LightType::Point;

            const float intensity =
                static_cast<float>(lightTable["intensity"].as_floating_point()->get());
            glm::vec3 color = ToVec3(*lightTable["color"].as_array());

            if (type == LightSystem::LightType::Directional) {
                glm::vec3 direction = ToVec3(*lightTable["direction"].as_array());
                LightSystem::CreateDirectionalLight(name, direction, color, intensity);
            } else {
                glm::vec3 position = ToVec3(*lightTable["position"].as_array());
                LightSystem::CreatePointLight(name, position, color, intensity);
            }
        }
    }

    void DeserialiseTransform(const SceneSystem::Entity *entity,
                              const toml::table &transformTable) {
        const auto position = ToVec3(*transformTable["position"].as_array());
        TransformSystem::SetPosition(entity->transform, position);

        const auto scale = ToVec3(*transformTable["scale"].as_array());
        TransformSystem::SetScale(entity->transform, scale);

        const auto rotation = ToVec3(*transformTable["rotation"].as_array());
        TransformSystem::SetRotation(entity->transform, rotation);
    }

    void DeserialiseMesh(const SceneSystem::Entity *entity,
                         const toml::table &meshTable) {
        const std::string meshName = meshTable["name"].as_string()->get();
        const std::string meshPath = meshTable["path"].as_string()->get();

        if (auto *existingMesh = MeshSystem::GetMesh(meshName)) {
            entity->mesh = existingMesh;
        } else {
            entity->mesh = ResourceManager::LoadMesh(meshName, meshPath);
        }
    }

    void DeserialiseTexture(const SceneSystem::Entity *entity,
                            const toml::table &textureTable) {
        const std::string textureName = textureTable["name"].as_string()->get();
        const std::string texturePath = textureTable["path"].as_string()->get();

        if (auto *existingTexture = TextureSystem::GetTexture(textureName)) {
            entity->texture = existingTexture;
        } else {
            entity->texture = ResourceManager::LoadTexture(textureName, texturePath);
        }
    }

    void DeserialiseMaterial(const SceneSystem::Entity *entity,
                             const toml::table &materialTable) {
        const std::string materialName = materialTable["name"].as_string()->get();

        std::string shaderName;

        if (materialTable.contains("shader")) {
            auto &shaderTable = *materialTable["shader"].as_table();
            shaderName = shaderTable["name"].as_string()->get();

            if (!ShaderSystem::GetShader(shaderName)) {
                const std::string vertPath =
                    shaderTable["vertex_path"].as_string()->get();
                const std::string fragPath =
                    shaderTable["fragment_path"].as_string()->get();

                ResourceManager::LoadShader(shaderName, vertPath, fragPath);
            }
        }

        MaterialSystem::Material *material = nullptr;
        if (auto *existingMaterial = MaterialSystem::GetMaterial(materialName)) {
            entity->material = existingMaterial;
            material = existingMaterial;
        } else {
            material = MaterialSystem::CreateMaterial(materialName, shaderName);
            entity->material = material;
        }

        if (materialTable.contains("properties")) {
            DeserialiseMaterialProperties(material,
                                          *materialTable["properties"].as_table());
        }
    }

    void DeserialiseMaterialProperties(MaterialSystem::Material *material,
                                       const toml::table &propertiesTable) {
        for (auto &[propKey, propValue] : propertiesTable) {
            std::string propName(propKey.str());

            if (propValue.is_floating_point()) {
                const auto value =
                    static_cast<float>(propValue.as_floating_point()->get());
                MaterialSystem::SetFloat(material, propName, value);
                continue;
            }

            if (propValue.is_integer()) {
                const auto value = propValue.as_integer()->get();
                MaterialSystem::SetInt(material, propName, value);
                continue;
            }

            if (propValue.is_array()) {
                auto &arr = *propValue.as_array();

                if (arr.size() == 2) {
                    glm::vec2 value = ToVec2(arr);
                    MaterialSystem::SetVec2(material, propName, value);
                    continue;
                }

                if (arr.size() == 3) {
                    glm::vec3 value = ToVec3(arr);
                    MaterialSystem::SetVec3(material, propName, value);
                    continue;
                }

                if (arr.size() == 4) {
                    glm::vec4 value = ToVec4(arr);
                    MaterialSystem::SetVec4(material, propName, value);
                    continue;
                }

                if (arr.size() == 16) {
                    glm::mat4 value(1.0f);
                    for (int i = 0; i < 4; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            value[i][j] = static_cast<float>(
                                arr[i * 4 + j].as_floating_point()->get());
                        }
                    }
                    MaterialSystem::SetMat4(material, propName, value);
                    continue;
                }
            }
        }
    }

    toml::table Read(const std::string &filename) {
        const std::string finalFilename = EnsureTomlExtension(filename);
        const std::filesystem::path filePath = GetScenesPath() / finalFilename;

        try {
            if (!std::filesystem::exists(filePath)) {
                ErrorHandler::Warn("Scene file does not exist: " + finalFilename,
                                   __FILE__, __func__, __LINE__);
                return {};
            }

            return toml::parse_file(filePath.string());
        } catch (const toml::parse_error &err) {
            ErrorHandler::Warn("Error parsing TOML: " + std::string(err.what()), __FILE__,
                               __func__, __LINE__);
            return {};
        } catch (const std::exception &err) {
            ErrorHandler::Warn("Error loading scene: " + std::string(err.what()),
                               __FILE__, __func__, __LINE__);
            return {};
        }
    }

    bool Write(const toml::table &data, const std::string &filename) {
        const std::string finalFilename = EnsureTomlExtension(filename);
        std::ofstream file(GetScenesPath() / finalFilename);

        if (!file.is_open()) {
            ErrorHandler::Warn("Failed to open file for writing: " + finalFilename,
                               __FILE__, __func__, __LINE__);
            return false;
        }

        file << data;
        file.close();

        return true;
    }
}
