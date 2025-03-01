#include "texture_system.h"

#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TextureSystem {
    static std::unordered_map<std::string, Texture> m_textures;

    static std::string GetTexturePath(const std::string &filename) {
        std::filesystem::path paths[] = {
            "Assets/Textures",
            "../Assets/Textures",
            "../../Assets/Textures",
            "gl-gfx/Assets/Textures"
        };

        for (const auto &basePath: paths) {
            if (std::filesystem::path fullPath = basePath / filename; exists(fullPath)) {
                return fullPath.string();
            }
        }

        return filename;
    }

    void Init() {
        m_textures.clear();
        stbi_set_flip_vertically_on_load(true);
    }

    Texture *CreateTexture(const std::string &name, const std::string &path, const bool generateMips) {
        Texture texture{};
        texture.name = name;
        texture.isValid = false;

        const std::filesystem::path textureFile = std::filesystem::path(path).filename();

        unsigned char *data = stbi_load(
            GetTexturePath(textureFile.string()).c_str(),
            &texture.width,
            &texture.height,
            &texture.channels,
            0
        );
        if (!data) {
            ErrorHandler::Warn("Failed to load texture: " + path, __FILE__, __func__, __LINE__);
            return nullptr;
        }

        switch (texture.channels) {
            case 1: texture.format = GL_RED;
                break;
            case 3: texture.format = GL_RGB;
                break;
            case 4: texture.format = GL_RGBA;
                break;
            default:
                ErrorHandler::Warn(
                    "Unsupported number of channels: " + std::to_string(texture.channels), __FILE__, __func__,
                    __LINE__);
                stbi_image_free(data);
                return nullptr;
        }

        texture.dataType = GL_UNSIGNED_BYTE;

        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            texture.format,
            texture.width,
            texture.height,
            0,
            texture.format,
            texture.dataType,
            data
        );

        if (generateMips) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        texture.path = path;
        texture.isValid = true;
        m_textures[name] = texture;

        return &m_textures[name];
    }

    Texture *CreateEmpty(const std::string &name, const int width, const int height, const GLenum format,
                         const GLenum dataType) {
        Texture texture{};
        texture.name = name;
        texture.width = width;
        texture.height = height;
        texture.format = format;
        texture.dataType = dataType;
        texture.isValid = false;

        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            GL_RGBA,
            dataType,
            nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        texture.isValid = true;
        m_textures[name] = texture;

        return &m_textures[name];
    }

    Texture *GetTexture(const std::string &name) {
        const auto it = m_textures.find(name);
        return (it != m_textures.end()) ? &it->second : nullptr;
    }

    void Bind(const Texture *texture, const uint32_t slot) {
        if (texture && texture->isValid) {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, texture->id);
        }
    }

    void Unbind(const uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void CleanUp() {
        for (auto &[name, texture]: m_textures) {
            if (texture.isValid) {
                glDeleteTextures(1, &texture.id);
            }
        }

        m_textures.clear();
    }
}
