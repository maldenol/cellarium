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
#include "./dear_imgui/imgui.h"
#include "./dear_imgui/imgui_impl_glfw.h"
#include "./dear_imgui/imgui_impl_opengl3.h"

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

// Processes statistics section in Dear ImGui window
void processStatistics(const CellEvolution::CellController &cellController) {
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

  // Statistics
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Statistics");

  // Adding statistics
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

// Processes controls section in Dear ImGui window
void processControls(GLFWwindow *window, Controls &controls) {
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

  // Controls
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Controls");

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
  ImGui::SliderInt(" Slider (Number of ticks per one rendering)", &controls.ticksPerRender, 1,
                   kMaxTicksPerRender);

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

// Processes simulation parameters section in Dear ImGui window (CellController friend function)
void CellEvolution::processSimulationParameters(CellEvolution::CellController &cellController) {
  // Constant
  constexpr int   kBigNumber             = 1000;
  constexpr float kButtonWidth           = 50.0f;
  const float     buttonHorizontalOffset = ImGui::GetWindowContentRegionWidth() - kButtonWidth;

  // Genome machine and simulation environment properties
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
                     "Genome machine and simulation environment properties");

  // _maxInstructionsPerTick
  ImGui::Text("Max instructions per tick: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Max instructions per tick)", &cellController._maxInstructionsPerTick,
                   1, cellController._genomeSize);
  // _maxAkinGenomDifference
  ImGui::Text("Max akin genom difference: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Max akin genom difference)", &cellController._maxAkinGenomDifference,
                   0, cellController._genomeSize);
  // _minChildEnergy
  ImGui::Text("Min child energy: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Min child energy)", &cellController._minChildEnergy, 1,
                   cellController._maxEnergy);
  // _maxEnergy
  ImGui::Text("Max energy: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Max energy)", &cellController._maxEnergy, 2, kBigNumber);
  // _maxBurstOfPhotosynthesisEnergy
  ImGui::Text("Max burst of photosynthesis energy: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Max burst of photosynthesis energy)",
                   &cellController._maxBurstOfPhotosynthesisEnergy, 0, cellController._maxEnergy);
  // _maxPhotosynthesisDepth
  ImGui::Text("Max photosynthesis depth: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Max photosynthesis depth)", &cellController._maxPhotosynthesisDepth, 0,
                   cellController._rows);
  // _summerDaytimeToWholeDayRatio
  ImGui::Text("Summer daytime to whole day ratio: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderFloat(" Slider (Summer daytime to whole day ratio)",
                     &cellController._summerDaytimeToWholeDayRatio, 0.0f, 1.0f);
  // _maxMinerals
  ImGui::Text("Max minerals: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Max minerals)", &cellController._maxMinerals, 0,
                   cellController._maxEnergy);
  // _maxBurstOfMinerals
  ImGui::Text("Max burst of minerals: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Max burst of minerals)", &cellController._maxBurstOfMinerals, 0,
                   cellController._maxMinerals);
  // _energyPerMineral
  ImGui::Text("Energy per mineral: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderFloat(" Slider (Energy per mineral)", &cellController._energyPerMineral, 0.0f,
                     cellController._maxEnergy);
  // _maxMineralHeight
  ImGui::Text("Max mineral height: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Max mineral height)", &cellController._maxMineralHeight, 0,
                   cellController._rows);
  // _maxBurstOfFoodEnergy
  ImGui::Text("Max burst of food energy: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Max burst of food energy)", &cellController._maxBurstOfFoodEnergy, 0,
                   cellController._maxEnergy);
  // _randomMutationChance
  ImGui::Text("Random mutation chance: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderFloat(" Slider (Random mutation chance)", &cellController._randomMutationChance,
                     0.0f, 1.0f);
  // _budMutationChance
  ImGui::Text("Bud mutation chance: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderFloat(" Slider (Bud mutation chance)", &cellController._budMutationChance, 0.0f,
                     1.0f);
  // _dayDurationInTicks
  ImGui::Text("Day duration in ticks: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Day duration in ticks)", &cellController._dayDurationInTicks, 1,
                   kBigNumber);
  // _seasonDurationInDays
  ImGui::Text("Season duration in days: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Season duration in days)", &cellController._seasonDurationInDays, 1,
                   kBigNumber);
  // _gammaFlashPeriodInDays
  ImGui::Text("Gamma flash period in days: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Gamma flash period in days)", &cellController._gammaFlashPeriodInDays,
                   1, kBigNumber);
  // _gammaFlashMaxMutationsCount
  ImGui::Text("Gamma flash max mutations count: ");
  ImGui::SameLine(buttonHorizontalOffset);
  ImGui::SetNextItemWidth(kButtonWidth);
  ImGui::SliderInt(" Slider (Gamma flash max mutations count)",
                   &cellController._gammaFlashMaxMutationsCount, 0, cellController._genomeSize);

  // Cell genome instruction enabling flags
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Cell genome instruction enabling flags");

  // _enableInstructionTurn
  ImGui::Text("Turn: %d", cellController._enableInstructionTurn);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Turn)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionTurn = !cellController._enableInstructionTurn;
  }
  // _enableInstructionMove
  ImGui::Text("Move: %d", cellController._enableInstructionMove);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Move)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionMove = !cellController._enableInstructionMove;
  }
  // _enableInstructionGetEnergyFromPhotosynthesis
  ImGui::Text("Get energy from photosynthesis: %d",
              cellController._enableInstructionGetEnergyFromPhotosynthesis);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Get energy from photosynthesis)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionGetEnergyFromPhotosynthesis =
        !cellController._enableInstructionGetEnergyFromPhotosynthesis;
  }
  // _enableInstructionGetEnergyFromMinerals
  ImGui::Text("Get energy from minerals: %d",
              cellController._enableInstructionGetEnergyFromMinerals);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Get energy from minerals)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionGetEnergyFromMinerals =
        !cellController._enableInstructionGetEnergyFromMinerals;
  }
  // _enableInstructionGetEnergyFromFood
  ImGui::Text("Get energy from food: %d", cellController._enableInstructionGetEnergyFromFood);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Get energy from food)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionGetEnergyFromFood =
        !cellController._enableInstructionGetEnergyFromFood;
  }
  // _enableInstructionBud
  ImGui::Text("Bud: %d", cellController._enableInstructionBud);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Bud)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionBud = !cellController._enableInstructionBud;
  }
  // _enableInstructionMutateRandomGene
  ImGui::Text("Mutate random gene: %d", cellController._enableInstructionMutateRandomGene);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Mutate random gene)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionMutateRandomGene =
        !cellController._enableInstructionMutateRandomGene;
  }
  // _enableInstructionShareEnergy
  ImGui::Text("Share energy: %d", cellController._enableInstructionShareEnergy);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Share energy)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionShareEnergy = !cellController._enableInstructionShareEnergy;
  }
  // _enableInstructionTouch
  ImGui::Text("Touch: %d", cellController._enableInstructionTouch);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Touch)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionTouch = !cellController._enableInstructionTouch;
  }
  // _enableInstructionDetermineEnergyLevel
  ImGui::Text("Determine energy level: %d", cellController._enableInstructionDetermineEnergyLevel);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Determine energy level)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionDetermineEnergyLevel =
        !cellController._enableInstructionDetermineEnergyLevel;
  }
  // _enableInstructionDetermineDepth
  ImGui::Text("Determine depth: %d", cellController._enableInstructionDetermineDepth);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Determine depth)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionDetermineDepth =
        !cellController._enableInstructionDetermineDepth;
  }
  // _enableInstructionDetermineBurstOfPhotosynthesisEnergy
  ImGui::Text("Determine burst of photosynthesis energy: %d",
              cellController._enableInstructionDetermineBurstOfPhotosynthesisEnergy);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Determine burst of photosynthesis energy)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionDetermineBurstOfPhotosynthesisEnergy =
        !cellController._enableInstructionDetermineBurstOfPhotosynthesisEnergy;
  }
  // _enableInstructionDetermineBurstOfMinerals
  ImGui::Text("Determine burst of minerals: %d",
              cellController._enableInstructionDetermineBurstOfMinerals);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Determine burst of minerals)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionDetermineBurstOfMinerals =
        !cellController._enableInstructionDetermineBurstOfMinerals;
  }
  // _enableInstructionDetermineBurstOfMineralEnergy
  ImGui::Text("Determine burst of mineral energy: %d",
              cellController._enableInstructionDetermineBurstOfMineralEnergy);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Determine burst of mineral energy)", {kButtonWidth, 0.0f})) {
    cellController._enableInstructionDetermineBurstOfMineralEnergy =
        !cellController._enableInstructionDetermineBurstOfMineralEnergy;
  }

  // Other simulation rule enabling flags
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Other simulation rule enabling flags");

  // _enableZeroEnergyOrganic
  ImGui::Text("Zero energy organic: %d", cellController._enableZeroEnergyOrganic);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Zero energy organic)", {kButtonWidth, 0.0f})) {
    cellController._enableZeroEnergyOrganic = !cellController._enableZeroEnergyOrganic;
  }
  // _enableForcedBuddingOnMaximalEnergyLevel
  ImGui::Text("Forced budding on maximal energy level: %d",
              cellController._enableForcedBuddingOnMaximalEnergyLevel);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Forced budding on maximal energy level)", {kButtonWidth, 0.0f})) {
    cellController._enableForcedBuddingOnMaximalEnergyLevel =
        !cellController._enableForcedBuddingOnMaximalEnergyLevel;
  }
  // _enableTryingToBudInUnoccupiedDirection
  ImGui::Text("Trying to bud in unoccupied direction: %d",
              cellController._enableTryingToBudInUnoccupiedDirection);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Trying to bud in unoccupied direction)", {kButtonWidth, 0.0f})) {
    cellController._enableTryingToBudInUnoccupiedDirection =
        !cellController._enableTryingToBudInUnoccupiedDirection;
  }
  // _enableDeathOnBuddingIfNotEnoughSpace
  ImGui::Text("Death on budding if not enough space: %d",
              cellController._enableDeathOnBuddingIfNotEnoughSpace);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Death on budding if not enough space)", {kButtonWidth, 0.0f})) {
    cellController._enableDeathOnBuddingIfNotEnoughSpace =
        !cellController._enableDeathOnBuddingIfNotEnoughSpace;
  }
  // _enableSeasons
  ImGui::Text("Seasons: %d", cellController._enableSeasons);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Seasons)", {kButtonWidth, 0.0f})) {
    cellController._enableSeasons = !cellController._enableSeasons;
  }
  // _enableDaytimes
  ImGui::Text("Daytimes: %d", cellController._enableDaytimes);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Daytimes)", {kButtonWidth, 0.0f})) {
    cellController._enableDaytimes = !cellController._enableDaytimes;
  }
  // _enableMaximizingFoodEnergy
  ImGui::Text("Maximizing food energy: %d", cellController._enableMaximizingFoodEnergy);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Maximizing food energy)", {kButtonWidth, 0.0f})) {
    cellController._enableMaximizingFoodEnergy = !cellController._enableMaximizingFoodEnergy;
  }
  // _enableDeadCellPinningOnSinking
  ImGui::Text("Dead cell pinning on sinking: %d", cellController._enableDeadCellPinningOnSinking);
  ImGui::SameLine(buttonHorizontalOffset);
  if (ImGui::Button("Toggle (Dead cell pinning on sinking)", {kButtonWidth, 0.0f})) {
    cellController._enableDeadCellPinningOnSinking =
        !cellController._enableDeadCellPinningOnSinking;
  }
}

// Processes genom overview section in Dear ImGui window
void processGenomOverview(CellEvolution::CellController &cellController) {}

// Processes Dear ImGui windows
void processDearImGui(GLFWwindow *window, Controls &controls,
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

    // Processing statistics section
    ImGui::BeginChild("Statistics", childSize, true);
    processStatistics(cellController);
    ImGui::EndChild();

    // Processing controls section
    ImGui::SameLine(0.0f, kBorderOffset);
    ImGui::BeginChild("Controls", childSize, true);
    processControls(window, controls);
    ImGui::EndChild();

    // Processing simulation parameters section
    ImGui::BeginChild("Simulation parameters", childSize, true);
    CellEvolution::processSimulationParameters(cellController);
    ImGui::EndChild();

    // Processing genom overview section
    ImGui::SameLine(0.0f, kBorderOffset);
    ImGui::BeginChild("Genom overview", childSize, true);
    processGenomOverview(cellController);
    ImGui::EndChild();

    ImGui::End();
  }

  // Rendering Dear ImGui window
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
