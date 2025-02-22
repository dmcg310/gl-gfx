#include "mesh_system.h"

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

        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);

        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        if (mesh.hasIndices) {
            glGenBuffers(1, &mesh.ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
        }

        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));

        // normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

        // texture coordinate attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_meshes[name] = mesh;

        return &m_meshes[name];
    }

    Mesh *GetMesh(const std::string &name) {
        const auto it = m_meshes.find(name);
        return (it != m_meshes.end()) ? &it->second : nullptr;
    }

    void Bind(const Mesh *mesh) {
        if (mesh) {
            glBindVertexArray(mesh->vao);
        }
    }

    void Unbind() {
        glBindVertexArray(0);
    }

    void Draw(const Mesh *mesh) {
        if (!mesh) {
            return;
        }

        if (mesh->hasIndices) {
            glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, nullptr);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
        }
    }

    void CleanUp() {
        for (auto &[name, mesh]: m_meshes) {
            glDeleteVertexArrays(1, &mesh.vao);
            glDeleteBuffers(1, &mesh.vbo);

            if (mesh.hasIndices) {
                glDeleteBuffers(1, &mesh.ebo);
            }
        }

        m_meshes.clear();
    }
}
