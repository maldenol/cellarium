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

// Enables fullscreen mode for given window
void enableFullscreenMode(GLFWwindow *window);

// Enables fullscreen mode for given window
void enableFullscreenMode(GLFWwindow *window, int &posX, int &posY, int &width, int &height);

// Disables fullscreen mode for given window
void disableFullscreenMode(GLFWwindow *window, int posX = 0, int posY = 0, int width = 0, int height = 0);

}

#endif
