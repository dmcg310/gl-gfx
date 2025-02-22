#pragma once

#include "common.h"

namespace MeshSystem {
    struct Mesh {
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        uint32_t vertexCount;
        uint32_t indexCount;
        bool hasIndices;
        std::string name;
    };

    void Init();

    Mesh *CreateMesh(
        const std::string &name,
        const std::vector<Vertex> &vertices,
        const std::vector<uint32_t> &indices = std::vector<uint32_t>()
    );

    Mesh *GetMesh(const std::string &name);

    void Bind(const Mesh *mesh);

    void Unbind();

    void Draw(const Mesh *mesh);

    void CleanUp();
}
