// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

// Header file
#include "./init_buffer.hpp"

// STD
#include <vector>

// CellController
#include "./cell_controller.hpp"

// Struct for storing position and color for rendering environment
struct EnvironmentRenderingData {
  float posX;
  float posY;
  float colorA;
};

// Initializes and configures OpenGL Vertex Array and Buffer Objects for cells
void initCellBuffers(int maxCount, GLuint &vao, GLuint &vbo) {
  // Creating and binding VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Creating, binding and configuring VBO
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<GLsizeiptr>(maxCount * sizeof(cellarium::CellController::CellRenderingData)),
      nullptr, GL_DYNAMIC_DRAW);
  glVertexAttribIPointer(0, 1, GL_INT, sizeof(cellarium::CellController::CellRenderingData),
                         reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(cellarium::CellController::CellRenderingData),
                        reinterpret_cast<void *>(sizeof(int)));
  glEnableVertexAttribArray(1);

  // Unbinding VAO and VBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Initializes and configures OpenGL Vertex Array and Buffer Objects for photosynthesis energy
void initPhotosynthesisEnergyBuffers(float minHeightInClipSpace, GLuint &vao, GLuint &vbo) {
  // Setting vertices and indices
  const std::vector<EnvironmentRenderingData> vertices{
      EnvironmentRenderingData{-1.0f,                 1.0f, 0.75f}, // top-left vertex
      EnvironmentRenderingData{ 1.0f,                 1.0f, 0.75f}, // top-right vertex
      EnvironmentRenderingData{-1.0f, minHeightInClipSpace,  0.0f}, // bottom-left vertex
      EnvironmentRenderingData{ 1.0f, minHeightInClipSpace,  0.0f}, // bottom-right vertex
  };
  const std::vector<unsigned int> indices{
      0, 1, 2,  // top-left triangle
      1, 3, 2,  // bottom-right triangle
  };

  // Creating and binding VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Creating, binding and configuring VBO
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(EnvironmentRenderingData)),
               &vertices[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(EnvironmentRenderingData),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(EnvironmentRenderingData),
                        reinterpret_cast<void *>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Creating, binding and configuring EBO
  GLuint ebo{};
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), &indices[0],
               GL_STATIC_DRAW);

  // Unbinding VAO and VBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Initializes and configures OpenGL Vertex Array and Buffer Objects for mineral energy
void initMineralEnergyBuffers(float maxHeightInClipSpace, GLuint &vao, GLuint &vbo) {
  // Setting vertices and indices
  const std::vector<EnvironmentRenderingData> vertices{
      EnvironmentRenderingData{-1.0f, maxHeightInClipSpace,  0.0f}, // top-left vertex
      EnvironmentRenderingData{ 1.0f, maxHeightInClipSpace,  0.0f}, // top-right vertex
      EnvironmentRenderingData{-1.0f,                -1.0f, 0.75f}, // bottom-left vertex
      EnvironmentRenderingData{ 1.0f,                -1.0f, 0.75f}, // bottom-right vertex
  };
  const std::vector<unsigned int> indices{
      0, 1, 2,  // top-left triangle
      1, 3, 2,  // bottom-right triangle
  };

  // Creating and binding VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Creating, binding and configuring VBO
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(EnvironmentRenderingData)),
               &vertices[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(EnvironmentRenderingData),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(EnvironmentRenderingData),
                        reinterpret_cast<void *>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Creating, binding and configuring EBO
  GLuint ebo{};
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), &indices[0],
               GL_STATIC_DRAW);

  // Unbinding VAO and VBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
