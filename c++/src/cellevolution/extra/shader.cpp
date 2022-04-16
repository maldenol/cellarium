// Header file
#include "./shader.hpp"

// STD
#include <iostream>

// Compiles shader with source code
void extra::compileShader(GLuint shader, const std::string &source) {
  // Setting shader's source and compiling it
  const char *sourceBuffer = source.c_str();
  glShaderSource(shader, 1, &sourceBuffer, nullptr);
  glCompileShader(shader);

  // Getting compilation success code
  GLint success{};
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  // Getting info log length
  GLint infoLogLength{};
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
  // Getting info log
  std::vector<char> infoLog{};
  infoLog.resize(infoLogLength + 1);
  glGetShaderInfoLog(shader, infoLogLength, nullptr, &infoLog[0]);
  // If compilation have failed
  if (success == GL_FALSE) {
    std::cout << "error: unable to compile shader #" << shader << "\n";
    // If there is an info log
    if (infoLogLength > 0) {
      std::cout << &infoLog[0];
    }
    // If there is no info log
    else {
      std::cout << "no error information";
    }
    std::cout << std::endl;
  }
}

// Creates shader program compiling and linking given shaders
GLuint extra::createShaderProgram(const std::vector<GLenum>      &shaderTypes,
                                  const std::vector<std::string> &shaderSources) {
  GLuint shaderProgram = glCreateProgram();

  // Getting shader count
  size_t shaderCount = shaderTypes.size();

  // If sizes mismatch
  if (shaderTypes.size() != shaderSources.size()) {
    return 0;
  }

  // Creating shader list
  std::vector<GLuint> shaders{};
  shaders.assign(shaderCount, 0);

  // Creating, attaching to shader program and compiling each shader
  for (size_t i = 0; i < shaderCount; ++i) {
    // Creating shader
    shaders[i] = glCreateShader(shaderTypes[i]);
    // Attaching shader to shader program
    glAttachShader(shaderProgram, shaders[i]);
    // Compiling shader
    compileShader(shaders[i], shaderSources[i]);
  }

  // Linking shader program
  glLinkProgram(shaderProgram);

  // Getting linkage success code
  GLint success{};
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  // Getting linkage info log length
  GLint infoLogLength{};
  glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
  // Getting linkage info log
  std::vector<char> infoLog{};
  infoLog.resize(infoLogLength + 1);
  glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, &infoLog[0]);
  // If linkage have failed
  if (success == GL_FALSE) {
    std::cout << "error: unable to link shaders in program #" << shaderProgram << "\n";
    // If there is an info log
    if (infoLogLength > 0) {
      std::cout << &infoLog[0];
    }
    // If there is no info log
    else {
      std::cout << "no error information";
    }
    std::cout << std::endl;
  }

  // Detaching from shader program and deleting each shader
  for (size_t i = 0; i < shaderCount; ++i) {
    // Detaching shader to shader program
    glDetachShader(shaderProgram, shaders[i]);
    // Deleting shader
    glDeleteShader(shaders[i]);
  }

  // If linkage have failed
  if (success == GL_FALSE) {
    // Deleting shader program
    glDeleteProgram(shaderProgram);

    return 0;
  }

  // Returning shader program
  return shaderProgram;
}
