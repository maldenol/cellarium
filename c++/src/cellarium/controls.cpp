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

// Header file
#include "./controls.hpp"

// STD
#include <algorithm>
#include <string>

// Dear ImGui
#include "dear_imgui/imgui.h"
#include "dear_imgui/imgui_impl_glfw.h"
#include "dear_imgui/imgui_impl_opengl3.h"

// "extra" internal library
#include "./extra/extra.hpp"

// Global constants
static constexpr int kMaxTicksPerRender = 1000;

// User input processing function
void processUserInput(GLFWwindow *window, Controls &controls) {
  // Static variables for fullscreen mode toggling
  static int sPosX{}, sPosY{}, sWidth{}, sHeight{};

  // Variables for key press and release handling
  static bool sPressed{};
  bool        released = true;

  // Switching cell rendering mode
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.cellRenderingMode = (controls.cellRenderingMode + 1) %
                                   static_cast<int>(CellEvolution::CellRenderingModes::Size);
    }
  }

  // Decreasing number of ticks per one rendering
  if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.ticksPerRender = std::max(controls.ticksPerRender - 1, 1);
    }
  }

  // Increasing number of ticks per one rendering
  if ((glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS &&
       (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) ||
      glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.ticksPerRender = std::min(controls.ticksPerRender + 1, kMaxTicksPerRender);
    }
  }

  // Toggling rendering environment flag
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableRenderingEnvironment = !controls.enableRenderingEnvironment;
    }
  }

  // Toggling rendering flag
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableRendering = !controls.enableRendering;
    }
  }

  // Toggling pause flag
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enablePause = !controls.enablePause;
    }
  }

  // Toggling window fullscreen mode
  if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableFullscreenMode = !controls.enableFullscreenMode;
      if (controls.enableFullscreenMode) {
        extra::enableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      } else {
        extra::disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      }
    }
  }

  // Disabling window fullscreen mode
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableFullscreenMode = false;
      if (glfwGetWindowMonitor(window) != nullptr) {
        extra::disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      }
    }
  }

  // Toggling window V-sync
  if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableVSync = !controls.enableVSync;
      if (controls.enableVSync) {
        glfwSwapInterval(1);
      } else {
        glfwSwapInterval(0);
      }
    }
  }

  // Toggling GUI flag
  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.enableGUI = !controls.enableGUI;
    }
  }

  // Requesting a tick
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      controls.tickRequest = true;
    }
  }

  // Cheking for key released
  if (released) {
    sPressed = false;
  }

  // Checking initial controls values
  static bool sFirstCall = true;
  if (sFirstCall) {
    sFirstCall = false;

    // Window fullscreen mode
    if (controls.enableFullscreenMode) {
      extra::enableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
    }

    // Window V-sync
    if (controls.enableVSync) {
      glfwSwapInterval(1);
    } else {
      glfwSwapInterval(0);
    }
  }
}

// Window size callback function
void windowSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

// Initializes Dear ImGui context
void initDearImGui(GLFWwindow *window) {
  // Creating Dear ImGui core context
  ImGui::CreateContext();
  // Initializing platform (GLFW) and render (OpenGL3) backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
}

// Terminates Dear ImGui context
void terminateDearImGui() {
  // Shutting render (OpenGL3) and platform (GLFW) backends down
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  // Destroying Dear ImGui core context
  ImGui::DestroyContext(ImGui::GetCurrentContext());
}

// Draws statistics section in Dear ImGui window
void drawStatistics(CellEvolution::CellController &cellController) {
  // Getting CellController statistics
  CellEvolution::CellController::Statistics statistics = cellController.getSimulationStatistics();

  // Static variables for FPS and TPS counters
  static int sFrameCount = 0;
  static int sFps        = 0;
  static int sTps        = 0;

  // Calculating FPS and TPS
  constexpr double kUpdateInterval = 0.2f;
  static double    sTime           = glfwGetTime();
  double           timePassed      = glfwGetTime() - sTime;

  ++sFrameCount;

  if (timePassed >= kUpdateInterval) {
    sFps        = static_cast<int>(static_cast<double>(sFrameCount) / timePassed);
    sFrameCount = 0;

    static int sTickCount  = 0;
    int        ticksPassed = statistics.tick - sTickCount;
    sTps                   = static_cast<int>(static_cast<double>(ticksPassed) / timePassed);
    sTickCount += ticksPassed;

    sTime += timePassed;
  }

  // Getting season name by its number
  std::string season{};
  switch (statistics.season) {
    case 0:
      season = "spring";
      break;
    case 1:
      season = "summer";
      break;
    case 2:
      season = "autumn";
      break;
    case 3:
      season = "winter";
      break;
  }

  // Adding text elements with statistics
  ImGui::Text("Frames per second:                     %d", sFps);
  ImGui::Text("Ticks  per second:                     %d", sTps);
  ImGui::Text("Tick:                                  %d", statistics.tick);
  ImGui::Text("Day:                                   %d", statistics.day);
  ImGui::Text("Year:                                  %d", statistics.year);
  ImGui::Text("Season:                                %s", season.c_str());
  ImGui::Text("Days to gamma flash:                   %d", statistics.daysToGammaFlash);
  ImGui::Text("Count of live cells:                   %d", statistics.countOfLiveCells);
  ImGui::Text("Count of dead cells:                   %d", statistics.countOfDeadCells);
  ImGui::Text("Count of buds:                         %ld", statistics.countOfBuds);
  ImGui::Text("Count of photosynthesis energy bursts: %ld",
              statistics.countOfPhotosynthesisEnergyBursts);
  ImGui::Text("Count of mineral energy bursts:        %ld", statistics.countOfMineralEnergyBursts);
  ImGui::Text("Count of food energy bursts:           %ld", statistics.countOfFoodEnergyBursts);
}

// Draws controls section in Dear ImGui window
void drawControls(GLFWwindow *window, Controls &controls) {
  // Constant
  constexpr float kButtonWidth           = 50.0f;
  const float     buttonHorizontalOffset = ImGui::GetWindowContentRegionWidth() - kButtonWidth;

  // Getting cell rendering mode name by its number
  std::string cellRenderingMode{};
  switch (controls.cellRenderingMode) {
    case 0:
      cellRenderingMode = "diet";
      break;
    case 1:
      cellRenderingMode = "energy level";
      break;
    case 2:
      cellRenderingMode = "energy sharing balance";
      break;
    case 3:
      cellRenderingMode = "last energy share";
      break;
  }

  // Switching cell rendering mode
  ImGui::Text("Cell rendering mode: %s", cellRenderingMode.c_str());
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Switch (Cell rendering mode)", {kButtonWidth, 0.0f})) {
    controls.cellRenderingMode = (controls.cellRenderingMode + 1) %
                                 static_cast<int>(CellEvolution::CellRenderingModes::Size);
  }

  // Setting number of ticks per one rendering
  ImGui::Text("Number of ticks per one rendering");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" (tpr)", &controls.ticksPerRender, 1, kMaxTicksPerRender);

  // Toggling rendering environment flag
  ImGui::Text("Rendering environment flag: %d", controls.enableRenderingEnvironment);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Rendering environment flag)", {kButtonWidth, 0.0f})) {
    controls.enableRenderingEnvironment = !controls.enableRenderingEnvironment;
  }

  // Toggling pause flag
  ImGui::Text("Pause flag: %d", controls.enablePause);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Pause flag)", {kButtonWidth, 0.0f})) {
    controls.enablePause = !controls.enablePause;
  }

  // Toggling window fullscreen mode
  ImGui::Text("Fullscreen mode flag: %d", controls.enableFullscreenMode);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Fullscreen mode flag)", {kButtonWidth, 0.0f})) {
    // Static variables for fullscreen mode toggling
    static int sPosX{}, sPosY{}, sWidth{}, sHeight{};

    controls.enableFullscreenMode = !controls.enableFullscreenMode;
    if (controls.enableFullscreenMode) {
      extra::enableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
    } else {
      extra::disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
    }
  }

  // Toggling window V-sync mode
  ImGui::Text("V-sync flag: %d", controls.enableVSync);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (V-sync flag)", {kButtonWidth, 0.0f})) {
    controls.enableVSync = !controls.enableVSync;
    if (controls.enableVSync) {
      glfwSwapInterval(1);
    } else {
      glfwSwapInterval(0);
    }
  }

  // Requesting a tick
  if (ImGui::Button("Request a tick")) {
    controls.tickRequest = true;
  }
}

// Draws simulation parameters section in Dear ImGui window
void drawSimulationParameters(CellEvolution::CellController &cellController) {}

// Draws genom overview section in Dear ImGui window
void drawGenomOverview(CellEvolution::CellController &cellController) {}

// Draws Dear ImGui windows
void drawDearImGui(GLFWwindow *window, Controls &controls,
                   CellEvolution::CellController &cellController) {
  // Preparing Dear ImGui for the new frame
  ImGui_ImplGlfw_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  // Configuring Dear ImGui window if GUI is enabled
  if (controls.enableGUI) {
    // Setting window size and position
    int width = 0, height = 0;
    glfwGetWindowSize(window, &width, &height);
    ImGui::SetNextWindowSize({static_cast<float>(width), static_cast<float>(height)});
    ImGui::SetNextWindowPos({0.0f, 0.0f});

    // Creating a window
    ImGui::Begin("Simulation Information & Controls", &controls.enableGUI,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    // Getting child (section) size
    constexpr float kBorderOffset = 4.0f;
    ImVec2          childSize     = ImGui::GetContentRegionAvail();
    childSize.x -= kBorderOffset;
    childSize.x /= 2.0f;
    childSize.y -= kBorderOffset;
    childSize.y /= 2.0f;

    // Drawing statistics section
    ImGui::BeginChild("Statistics", childSize, true);
    drawStatistics(cellController);
    ImGui::EndChild();

    // Drawing controls section
    ImGui::SameLine(0.0f, kBorderOffset);
    ImGui::BeginChild("Controls", childSize, true);
    drawControls(window, controls);
    ImGui::EndChild();

    // Drawing simulation parameters section
    ImGui::BeginChild("Simulation parameters", childSize, true);
    drawSimulationParameters(cellController);
    ImGui::EndChild();

    // Drawing genom overview section
    ImGui::SameLine(0.0f, kBorderOffset);
    ImGui::BeginChild("Genom overview", childSize, true);
    drawGenomOverview(cellController);
    ImGui::EndChild();

    ImGui::End();
  }

  // Rendering Dear ImGui window
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
