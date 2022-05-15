// This file is part of cellarium.
//
// cellarium is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cellarium is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with cellarium.  If not, see <https://www.gnu.org/licenses/>.

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
