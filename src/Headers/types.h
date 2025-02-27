#pragma once

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

struct InstanceData {
  glm::mat4 modelMatrix;
  glm::vec4 color;
};
