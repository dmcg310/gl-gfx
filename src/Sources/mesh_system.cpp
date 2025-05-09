#include "mesh_system.h"

#include <unordered_map>

namespace MeshSystem {
    static std::unordered_map<std::string, Mesh> m_meshes;

    void Init() {
        m_meshes.clear();
    }

    Mesh *CreateMesh(const std::string &name, const std::vector<Vertex> &vertices,
                     const std::vector<uint32_t> &indices) {
        Mesh mesh{};
        mesh.name = name;
        mesh.hasIndices = !indices.empty();
        mesh.indexCount = indices.size();
        mesh.vertexCount = vertices.size();

        mesh.minBounds = glm::vec3(std::numeric_limits<float>::max());
        mesh.maxBounds = glm::vec3(std::numeric_limits<float>::lowest());
        for (const auto &vertex : vertices) {
            mesh.minBounds = glm::min(mesh.minBounds, vertex.position);
            mesh.maxBounds = glm::max(mesh.maxBounds, vertex.position);
        }

        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);

        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(),
                     GL_STATIC_DRAW);

        if (mesh.hasIndices) {
            glGenBuffers(1, &mesh.ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
                         indices.data(), GL_STATIC_DRAW);
        }

        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, position));

        // normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, normal));

        // texture coordinate attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, texCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_meshes[name] = mesh;

        return &m_meshes[name];
    }

    Mesh *GetMesh(const std::string &name) {
        const auto it = m_meshes.find(name);
        return (it != m_meshes.end()) ? &it->second : nullptr;
    }

    void SetupInstancedMesh(Mesh *mesh, uint32_t maxInstances) {
        if (!mesh) {
            return;
        }

        mesh->maxInstances = maxInstances;
        mesh->instanceCount = 0;
        mesh->isInstanced = true;

        glGenBuffers(1, &mesh->instanceVBO);
        glBindVertexArray(mesh->vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->instanceVBO);

        glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(InstanceData), nullptr,
                     GL_DYNAMIC_DRAW);

        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(3 + i);
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                                  (void *)(sizeof(float) * i * 4));
            glVertexAttribDivisor(3 + i, 1);
        }

        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                              (void *)(sizeof(glm::mat4)));
        glVertexAttribDivisor(7, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void UpdateInstanceData(Mesh *mesh, const std::vector<InstanceData> &instances) {
        if (!mesh || !mesh->isInstanced) {
            return;
        }

        mesh->instanceCount = std::min((uint32_t)instances.size(), mesh->maxInstances);
        if (mesh->instanceCount == 0) {
            return;
        }

        glBindBuffer(GL_ARRAY_BUFFER, mesh->instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->instanceCount * sizeof(InstanceData),
                        instances.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Bind(const Mesh *mesh) {
        if (mesh) {
            glBindVertexArray(mesh->vao);
        }
    }

    void Unbind() {
        glBindVertexArray(0);
    }

    void DrawInstanced(const Mesh *mesh) {
        if (!mesh || !mesh->isInstanced || mesh->instanceCount == 0) {
            return;
        }

        if (mesh->hasIndices) {
            glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT,
                                    nullptr, mesh->instanceCount);
        } else {
            glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->vertexCount,
                                  mesh->instanceCount);
        }
    }

    void CleanUp() {
        for (auto &[name, mesh] : m_meshes) {
            glDeleteVertexArrays(1, &mesh.vao);
            glDeleteBuffers(1, &mesh.vbo);

            if (mesh.hasIndices) {
                glDeleteBuffers(1, &mesh.ebo);
            }
        }

        m_meshes.clear();
    }
}
