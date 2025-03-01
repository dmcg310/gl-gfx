#include "serialisation.h"

#include "camera_system.h"
#include "scene_system.h"
#include "transform_system.h"
#include "light_system.h"

namespace Serialisation {
    bool Serialise(const std::string &filename, const std::string &sceneName) {
        toml::table scene;
        scene.insert("scene_name", sceneName);

        toml::table camera;
        if (const auto *mainCamera = CameraSystem::GetMainCamera()) {
            camera.insert("name", mainCamera->name);
            camera.insert("position", ToTomlArray(mainCamera->position));
            camera.insert("front", ToTomlArray(mainCamera->front));
            camera.insert("up", ToTomlArray(mainCamera->up));
            camera.insert("yaw", mainCamera->yaw);
            camera.insert("pitch", mainCamera->pitch);
            camera.insert("fov", mainCamera->fov);
            camera.insert("aspect_ratio", mainCamera->aspectRatio);
            camera.insert("near_plane", mainCamera->nearPlane);
            camera.insert("far_plane", mainCamera->farPlane);
        }
        scene.insert("camera", camera);

        toml::array entities;
        for (const auto *entity: SceneSystem::GetAllEntities()) {
            toml::table entityTable;
            entityTable.insert("name", entity->name);
            entityTable.insert("is_active", entity->isActive);
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
                entityTable.insert("mesh", entity->mesh->name);
            }

            if (entity->texture) {
                entityTable.insert("texture", entity->texture->name);
            }

            if (entity->material) {
                toml::table material;
                material.insert("name", entity->material->name);

                if (entity->material->shader) {
                    material.insert("shader", entity->material->shader->name);
                }

                toml::table properties;
                for (const auto &[name, prop]: entity->material->properties) {
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
                            ErrorHandler::Warn("Unknown property type", __FILE__, __func__, __LINE__);
                    }
                }

                material.insert("properties", properties);

                entityTable.insert("material", material);
            }

            entities.push_back(entityTable);
        }
        scene.insert("entity", entities);

        toml::array lights;
        for (const auto *light: LightSystem::GetAllLights()) {
            toml::table lightTable;
            lightTable.insert("name", light->name);
            lightTable.insert("type", light->type == LightSystem::LightType::Directional ? "directional" : "point");
            lightTable.insert("position", ToTomlArray(light->position));
            if (light->type == LightSystem::LightType::Directional) {
                lightTable.insert("direction", ToTomlArray(light->direction));
            }
            lightTable.insert("color", ToTomlArray(light->color));
            lightTable.insert("intensity", light->intensity);
            lightTable.insert("is_active", light->isActive);

            lights.push_back(lightTable);
        }
        scene.insert("light", lights);

        return Write(scene, filename);
    }

    bool Deserialise(const std::string &filename) {
        return false;
    }

    void Default() {
    }

    bool Write(const toml::table &data, const std::string &filename) {
        const std::string finalFilename = EnsureTomlExtension(filename);
        std::ofstream file(GetScenesPath() / finalFilename);

        if (!file.is_open()) {
            ErrorHandler::Warn("Failed to open file for writing: " + finalFilename, __FILE__, __func__, __LINE__);
            return false;
        }

        file << data;
        file.close();

        return true;
    }
}
