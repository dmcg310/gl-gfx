#pragma once

#include "common.h"

#include "scene_system.h"

#include <toml++/toml.hpp>
#include <filesystem>

namespace Serialisation {
    bool Serialise(const std::string &filename, const std::string &sceneName);

    bool Deserialise(const std::string &filename);

    bool DeserialiseCamera(const toml::table &scene);

    bool DeserialiseEntities(const toml::table &scene);

    void DeserialiseLights(const toml::table &scene);

    void DeserialiseTransform(const SceneSystem::Entity *entity, const toml::table &transformTable);

    void DeserialiseMesh(const SceneSystem::Entity *entity, const toml::table &meshTable);

    void DeserialiseTexture(const SceneSystem::Entity *entity, const toml::table &textureTable);

    void DeserialiseMaterial(const SceneSystem::Entity *entity, const toml::table &materialTable);

    void DeserialiseMaterialProperties(MaterialSystem::Material *material, const toml::table &propertiesTable);

    toml::table Read(const std::string &filename);

    bool Write(const toml::table &data, const std::string &filename);

    inline toml::array ToTomlArray(const glm::vec2 &vec) {
        return toml::array{vec.x, vec.y};
    }

    inline toml::array ToTomlArray(const glm::vec3 &vec) {
        return toml::array{vec.x, vec.y, vec.z};
    }

    inline toml::array ToTomlArray(const glm::vec4 &vec) {
        return toml::array{vec.x, vec.y, vec.z, vec.w};
    }

    inline toml::array ToTomlArray(const glm::mat4 &mat) {
        toml::array arr;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                arr.push_back(mat[i][j]);
            }
        }

        return arr;
    }

    inline glm::vec2 ToVec2(const toml::array &arr) {
        if (arr.size() < 2) {
            return glm::vec2(0.0f);
        }

        return {
            arr[0].value_or(0.0f),
            arr[1].value_or(0.0f)
        };
    }

    inline glm::vec3 ToVec3(const toml::array &arr) {
        if (arr.size() < 3) {
            return glm::vec3(0.0f);
        }

        return {
            arr[0].value_or(0.0f),
            arr[1].value_or(0.0f),
            arr[2].value_or(0.0f)
        };
    }

    inline glm::vec4 ToVec4(const toml::array &arr) {
        if (arr.size() < 4) {
            return glm::vec4(1.0f);
        }

        return {
            arr[0].value_or(1.0f),
            arr[1].value_or(1.0f),
            arr[2].value_or(1.0f),
            arr[3].value_or(1.0f)
        };
    }

    inline std::string EnsureTomlExtension(const std::string &filename) {
        if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".toml") {
            return filename + ".toml";
        }

        return filename;
    }

    inline std::filesystem::path GetScenesPath() {
        std::filesystem::path paths[] = {
            "Scenes",
            "../Scenes",
            "../../Scenes",
            "gl-gfx/Scenes"
        };

        for (const auto &path: paths) {
            if (std::filesystem::exists(path)) {
                return path;
            }
        }

        ErrorHandler::Warn(
            "Couldn't determine scenes directory, using CWD: " + std::filesystem::current_path().string(),
            __FILE__, __func__, __LINE__);

        return "";
    }
}
