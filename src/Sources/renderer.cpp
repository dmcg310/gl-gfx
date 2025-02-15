#include "renderer.h"
#include "shader_manager.h"

#include <vector>

namespace Renderer {
    std::vector<Vertex> m_vertices;
    GLuint m_VBO = 0;
    GLuint m_VAO = 0;
    GLuint m_shaderProgram = 0;

    void Init() {
        m_shaderProgram = ShaderManager::CreateProgram(
            "../src/Shaders/triangle.vert",
            "../src/Shaders/triangle.frag"
        );
        if (m_shaderProgram == 0) {
            ErrorHandler::ThrowError(
                "Failed to create shader program",
                __FILE__, __func__, __LINE__
            );
        }

        glGenBuffers(1, &m_VBO);
        glGenVertexArrays(1, &m_VAO);

        m_vertices = {
            Vertex(-0.5f, -0.5f, 0.0f),
            Vertex(0.5f, -0.5f, 0.0f),
            Vertex(0.0f, 0.5f, 0.0f)
        };

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            m_vertices.size() * sizeof(Vertex),
            m_vertices.data(),
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Render() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_shaderProgram);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    }

    void CleanUp() {
        if (m_VBO != 0) {
            glDeleteBuffers(1, &m_VBO);
        }

        if (m_VAO != 0) {
            glDeleteVertexArrays(1, &m_VAO);
        }

        if (m_shaderProgram != 0) {
            glDeleteProgram(m_shaderProgram);
        }

        ShaderManager::CleanUp();

        m_VBO = 0;
        m_VAO = 0;
        m_shaderProgram = 0;
    }
}
