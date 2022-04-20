// This file is part of cellevolution.
//
// cellevolution is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cellevolution is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with cellevolution.  If not, see <https://www.gnu.org/licenses/>.

// Header file
#include "./glinit.hpp"

// STD
#include <iostream>

// OpenGL
#include <glad/glad.h>

// Initializes GLFW and returns configured window with OpenGL context
GLFWwindow *extra::createWindow(int width, int height, const std::string &title,
                                int openGLVersionMajor, int openGLVersionMinor) {
  // Initializing GLFW
  if (glfwInit() == GLFW_FALSE) {
    std::cout << "error: failed to initialize GLFW" << std::endl;

    return nullptr;
  }

  // Setting OpenGL version and profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openGLVersionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openGLVersionMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Creating GLFW window
  GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "error: failed to create GLFW window" << std::endl;

    glfwTerminate();
    return nullptr;
  }

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Loading OpenGL functions with GLAD
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "error: failed to load OpenGL functions using GLAD" << std::endl;

    glfwMakeContextCurrent(nullptr);
    glfwTerminate();
    return nullptr;
  }

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Returning window
  return window;
}

// Terminates window with OpenGL context and GLFW
void extra::terminateWindow(GLFWwindow *window) {
  // Closing GLFW window
  glfwSetWindowShouldClose(window, true);

  // Terminating GLFW
  glfwTerminate();
}

// Enables fullscreen mode for given window
void extra::enableFullscreenMode(GLFWwindow *window) {
  GLFWmonitor       *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode    = glfwGetVideoMode(monitor);
  glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

// Enables fullscreen mode for given window
void extra::enableFullscreenMode(GLFWwindow *window, int &posX, int &posY, int &width,
                                 int &height) {
  glfwGetWindowPos(window, &posX, &posY);
  glfwGetWindowSize(window, &width, &height);
  enableFullscreenMode(window);
}

// Disables fullscreen mode for given window
void extra::disableFullscreenMode(GLFWwindow *window, int posX, int posY, int width, int height) {
  GLFWmonitor       *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode    = glfwGetVideoMode(monitor);
  if (width == 0 || height == 0) {
    width  = mode->width;
    height = mode->height;
  }
  glfwSetWindowMonitor(window, nullptr, posX, posY, width, height, mode->refreshRate);
}
