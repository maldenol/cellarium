#ifndef EXTRA_GLINIT_HPP
#define EXTRA_GLINIT_HPP

// STD
#include <string>

// OpenGL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace extra {

// Initializes GLFW and returns configured window with OpenGL context
GLFWwindow *createWindow(int width, int height, const std::string &title, int openGLVersionMajor, int openGLVersionMinor);

// Terminates window with OpenGL context and GLFW
void terminateWindow(GLFWwindow *window);

}

#endif
