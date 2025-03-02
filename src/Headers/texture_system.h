#pragma once

#include "common.h"

namespace TextureSystem {
    struct Texture {
        GLuint id;
        std::string name;
        int width;
        int height;
        int channels;
        GLenum format;
        GLenum dataType;
        bool isValid;
        std::string path;
    };

    void Init();

    Texture *CreateTexture(const std::string &name, const std::string &path,
                           bool generateMips = true);

    Texture *CreateEmpty(const std::string &name, int width, int height,
                         GLenum format = GL_RGBA8, GLenum dataType = GL_UNSIGNED_BYTE);

    Texture *GetTexture(const std::string &name);

    void Bind(const Texture *texture, uint32_t slot = 0);

    void Unbind(uint32_t slot = 0);

    void CleanUp();
}
