#pragma once

// struct Vertex {
//   float x, y, z; // Position
//   float u, v;    // Texture coordinates
//
//   Vertex() = default;
//   Vertex(float x, float y, float z, float u = 0.0f, float v = 0.0f)
//       : x(x), y(y), z(z), u(u), v(v) {}
// };

struct Vertex {
  float x, y, z;

  Vertex() = default;

  Vertex(float x, float y, float z) : x(x), y(y), z(z) {
  }
};
