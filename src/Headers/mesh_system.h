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
        GLuint instanceVBO = 0;
        uint32_t maxInstances = 0;
        uint32_t instanceCount = 0;
        bool isInstanced;
    };

    void Init();

    Mesh *CreateMesh(
        const std::string &name,
        const std::vector<Vertex> &vertices,
        const std::vector<uint32_t> &indices = std::vector<uint32_t>()
    );

    Mesh *GetMesh(const std::string &name);

    void SetupInstancedMesh(Mesh *mesh, uint32_t maxInstances);

    void UpdateInstanceData(Mesh *mesh, const std::vector<InstanceData> &instances);

    void Bind(const Mesh *mesh);

    void Unbind();

    void DrawInstanced(const Mesh *mesh);

    void CleanUp();
}
