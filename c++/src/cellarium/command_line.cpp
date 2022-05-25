// Copyright 2021 Malovanyi Denys Olehovych (maldenol)
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

// Header file
#include "./command_line.hpp"

// STD
#include <algorithm>
#include <iostream>

// Qt
#include <QByteArray>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QStringList>

// Outputs version and license
void outputVersionAndLicense() {
  std::cout << "cellarium (maldenol) 1.0.0\n";
  std::cout << "Copyright (c) 2021 Malovanyi Denys Olehovych (maldenol)" << std::endl;
}

// Outputs controls help
void outputControlsHelp() {
  std::cout << "Controls help\n";
  std::cout << "Switch cell rendering mode:                              m\n";
  std::cout << "Decrease/increase number of ticks per one rendering:     -/+\n";
  std::cout << "Toggle environment rendering:                            e\n";
  std::cout << "Toggle rendering:                                        r\n";
  std::cout << "Toggle pause:                                            p\n";
  std::cout << "Toggle fullscreen mode:                                  f11\n";
  std::cout << "Disable fullscren mode:                                  esc\n";
  std::cout << "Toggle V-sync:                                           v\n";
  std::cout << "Toggle GUI:                                              tab\n";
  std::cout << "Request a tick:                                          space" << std::endl;
}

// Generates default configuration file
int generateDefaultConfigurationFile() {
  // Opening config file
  QString configFileName{QString::number(QDateTime::currentMSecsSinceEpoch()) + ".json"};
  QFile   configFile{configFileName};
  if (!configFile.open(QFile::WriteOnly | QFile::Text)) {
    std::cout << "error: cannot create " << configFileName.toStdString() << std::endl;
    return -1;
  } else {
    std::cout << "Successfully generated config file \"" << configFileName.toStdString() << "\"."
              << std::endl;
  }

  // Setting default configuration
  QJsonObject configJsonObject{};
  configJsonObject.insert("cellRenderingMode", 0);
  configJsonObject.insert("ticksPerRender", 1);
  configJsonObject.insert("enableRendering", true);
  configJsonObject.insert("enableRenderingEnvironment", true);
  configJsonObject.insert("enablePause", false);
  configJsonObject.insert("enableFullscreenMode", false);
  configJsonObject.insert("enableVSync", true);
  configJsonObject.insert("maxPhotosynthesisDepthMultiplier",
                          cellarium::kInitPhotosynthesisDepthMultiplier);
  configJsonObject.insert("maxMineralHeightMultiplier", cellarium::kInitMineralHeightMultiplier);
  configJsonObject.insert("randomSeed", static_cast<int>(cellarium::kInitRandomSeed));
  configJsonObject.insert("width", cellarium::kInitWidth);
  configJsonObject.insert("height", cellarium::kInitHeight);
  configJsonObject.insert("cellSize", cellarium::kInitCellSize);
  configJsonObject.insert("genomeSize", cellarium::kInitGenomSize);
  configJsonObject.insert("maxInstructionsPerTick", cellarium::kInitMaxInstructionsPerTick);
  configJsonObject.insert("maxAkinGenomDifference", cellarium::kInitMaxAkinGenomDifference);
  configJsonObject.insert("minChildEnergy", cellarium::kInitMinChildEnergy);
  configJsonObject.insert("maxEnergy", cellarium::kInitMaxEnergy);
  configJsonObject.insert("maxBurstOfPhotosynthesisEnergy",
                          cellarium::kInitMaxBurstOfPhotosynthesisEnergy);
  configJsonObject.insert("summerDaytimeToWholeDayRatio",
                          cellarium::kInitSummerDaytimeToWholeDayRatio);
  configJsonObject.insert("maxMinerals", cellarium::kInitMaxMinerals);
  configJsonObject.insert("maxBurstOfMinerals", cellarium::kInitMaxBurstOfMinerals);
  configJsonObject.insert("energyPerMineral", cellarium::kInitEnergyPerMineral);
  configJsonObject.insert("maxBurstOfFoodEnergy", cellarium::kInitMaxBurstOfFoodEnergy);
  configJsonObject.insert("childBudMutationChance", cellarium::kInitChildBudMutationChance);
  configJsonObject.insert("parentBudMutationChance", cellarium::kInitParentBudMutationChance);
  configJsonObject.insert("randomMutationChance", cellarium::kInitRandomMutationChance);
  configJsonObject.insert("dayDurationInTicks", cellarium::kInitDayDurationInTicks);
  configJsonObject.insert("seasonDurationInDays", cellarium::kInitSeasonDurationInDays);
  configJsonObject.insert("gammaFlashPeriodInDays", cellarium::kInitGammaFlashPeriodInDays);
  configJsonObject.insert("gammaFlashMaxMutationsCount",
                          cellarium::kInitGammaFlashMaxMutationsCount);
  configJsonObject.insert("enableInstructionTurn", cellarium::kInitEnableInstructionTurn);
  configJsonObject.insert("enableInstructionMove", cellarium::kInitEnableInstructionMove);
  configJsonObject.insert("enableInstructionGetEnergyFromPhotosynthesis",
                          cellarium::kInitEnableInstructionGetEnergyFromPhotosynthesis);
  configJsonObject.insert("enableInstructionGetEnergyFromMinerals",
                          cellarium::kInitEnableInstructionGetEnergyFromMinerals);
  configJsonObject.insert("enableInstructionGetEnergyFromFood",
                          cellarium::kInitEnableInstructionGetEnergyFromFood);
  configJsonObject.insert("enableInstructionBud", cellarium::kInitEnableInstructionBud);
  configJsonObject.insert("enableInstructionMutateRandomGene",
                          cellarium::kInitEnableInstructionMutateRandomGene);
  configJsonObject.insert("enableInstructionShareEnergy",
                          cellarium::kInitEnableInstructionShareEnergy);
  configJsonObject.insert("enableInstructionTouch", cellarium::kInitEnableInstructionTouch);
  configJsonObject.insert("enableInstructionDetermineEnergyLevel",
                          cellarium::kInitEnableInstructionDetermineEnergyLevel);
  configJsonObject.insert("enableInstructionDetermineDepth",
                          cellarium::kInitEnableInstructionDetermineDepth);
  configJsonObject.insert("enableInstructionDetermineBurstOfPhotosynthesisEnergy",
                          cellarium::kInitenableInstructionDetermineBurstOfPhotosynthesisEnergy);
  configJsonObject.insert("enableInstructionDetermineBurstOfMinerals",
                          cellarium::kInitEnableInstructionDetermineBurstOfMinerals);
  configJsonObject.insert("enableInstructionDetermineBurstOfMineralEnergy",
                          cellarium::kInitEnableInstructionDetermineBurstOfMineralEnergy);
  configJsonObject.insert("enableZeroEnergyOrganic", cellarium::kInitEnableZeroEnergyOrganic);
  configJsonObject.insert("enableForcedBuddingOnMaximalEnergyLevel",
                          cellarium::kInitEnableForcedBuddingOnMaximalEnergyLevel);
  configJsonObject.insert("enableTryingToBudInUnoccupiedDirection",
                          cellarium::kInitEnableTryingToBudInUnoccupiedDirection);
  configJsonObject.insert("enableDeathOnBuddingIfNotEnoughSpace",
                          cellarium::kInitEnableDeathOnBuddingIfNotEnoughSpace);
  configJsonObject.insert("enableSeasons", cellarium::kInitEnableSeasons);
  configJsonObject.insert("enableDaytimes", cellarium::kInitEnableDaytimes);
  configJsonObject.insert("enableMaximizingFoodEnergy", cellarium::kInitEnableMaximizingFoodEnergy);
  configJsonObject.insert("enableDeadCellPinningOnSinking",
                          cellarium::kInitEnableDeadCellPinningOnSinking);
  QJsonArray firstCellGenomeArray;
  for (int firstCellGenomeInstruction : cellarium::kInitFirstCellGenome) {
    firstCellGenomeArray.append(firstCellGenomeInstruction);
  }
  configJsonObject.insert("firstCellGenome", firstCellGenomeArray);
  configJsonObject.insert("firstCellEnergyMultiplier", cellarium::kInitFirstCellEnergyMultiplier);
  configJsonObject.insert("firstCellDirection", cellarium::kInitFirstCellDirection);
  configJsonObject.insert("firstCellIndexMultiplier", cellarium::kInitFirstCellIndexMultiplier);

  // Writing configuration to file
  QJsonDocument configJsonDocument{configJsonObject};
  configFile.write(configJsonDocument.toJson());

  return 0;
}

// Load configuration file
int loadConfigurationFile(const QString &filename, Controls &controls,
                          cellarium::CellController::Params &cellControllerParams) {
  // Opening config file
  QFile configFile{filename};
  if (!configFile.open(QFile::ReadOnly | QFile::Text)) {
    std::cout << "error: cannot open " << filename.toStdString() << std::endl;
    return -1;
  }

  // Reading configuration from file
  QByteArray    configByteArray{configFile.readAll()};
  QJsonDocument configJsonDocument{QJsonDocument::fromJson(configByteArray)};
  QJsonObject   configJsonObject{configJsonDocument.object()};

  // Updating Controls
  controls.cellRenderingMode = configJsonObject.contains("cellRenderingMode")
                                   ? configJsonObject["cellRenderingMode"].toInt()
                                   : controls.cellRenderingMode;
  controls.ticksPerRender    = configJsonObject.contains("ticksPerRender")
                                   ? configJsonObject["ticksPerRender"].toInt()
                                   : controls.ticksPerRender;
  controls.enableRendering   = configJsonObject.contains("enableRendering")
                                   ? configJsonObject["enableRendering"].toBool()
                                   : controls.enableRendering;
  controls.enableRenderingEnvironment =
      configJsonObject.contains("enableRenderingEnvironment")
          ? configJsonObject["enableRenderingEnvironment"].toBool()
          : controls.enableRenderingEnvironment;
  controls.enablePause          = configJsonObject.contains("enablePause")
                                      ? configJsonObject["enablePause"].toBool()
                                      : controls.enablePause;
  controls.enableFullscreenMode = configJsonObject.contains("enableFullscreenMode")
                                      ? configJsonObject["enableFullscreenMode"].toBool()
                                      : controls.enableFullscreenMode;
  controls.enableVSync          = configJsonObject.contains("enableVSync")
                                      ? configJsonObject["enableVSync"].toBool()
                                      : controls.enableVSync;

  // Updating CellController::Params
  cellControllerParams.mersenneTwisterEngine =
      configJsonObject.contains("randomSeed")
          ? std::mt19937{static_cast<unsigned int>(configJsonObject["randomSeed"].toInt())}
          : cellControllerParams.mersenneTwisterEngine;
  cellControllerParams.randomSeed = configJsonObject.contains("randomSeed")
                                        ? configJsonObject["randomSeed"].toInt()
                                        : cellControllerParams.randomSeed;
  cellControllerParams.width      = configJsonObject.contains("width")
                                        ? configJsonObject["width"].toInt()
                                        : cellControllerParams.width;
  cellControllerParams.height     = configJsonObject.contains("height")
                                        ? configJsonObject["height"].toInt()
                                        : cellControllerParams.height;
  cellControllerParams.cellSize   = configJsonObject.contains("cellSize")
                                        ? static_cast<float>(configJsonObject["cellSize"].toDouble())
                                        : cellControllerParams.cellSize;
  cellControllerParams.maxPhotosynthesisDepthMultiplier =
      configJsonObject.contains("maxPhotosynthesisDepthMultiplier")
          ? static_cast<float>(configJsonObject["maxPhotosynthesisDepthMultiplier"].toDouble())
          : cellControllerParams.maxPhotosynthesisDepthMultiplier;
  cellControllerParams.maxMineralHeightMultiplier =
      configJsonObject.contains("maxMineralHeightMultiplier")
          ? static_cast<float>(configJsonObject["maxMineralHeightMultiplier"].toDouble())
          : cellControllerParams.maxMineralHeightMultiplier;
  cellControllerParams.genomeSize = configJsonObject.contains("genomeSize")
                                        ? configJsonObject["genomeSize"].toInt()
                                        : cellControllerParams.genomeSize;
  cellControllerParams.maxInstructionsPerTick =
      configJsonObject.contains("maxInstructionsPerTick")
          ? configJsonObject["maxInstructionsPerTick"].toInt()
          : cellControllerParams.maxInstructionsPerTick;
  cellControllerParams.maxAkinGenomDifference =
      configJsonObject.contains("maxAkinGenomDifference")
          ? configJsonObject["maxAkinGenomDifference"].toInt()
          : cellControllerParams.maxAkinGenomDifference;
  cellControllerParams.minChildEnergy = configJsonObject.contains("minChildEnergy")
                                            ? configJsonObject["minChildEnergy"].toInt()
                                            : cellControllerParams.minChildEnergy;
  cellControllerParams.maxEnergy      = configJsonObject.contains("maxEnergy")
                                            ? configJsonObject["maxEnergy"].toInt()
                                            : cellControllerParams.maxEnergy;
  cellControllerParams.maxBurstOfPhotosynthesisEnergy =
      configJsonObject.contains("maxBurstOfPhotosynthesisEnergy")
          ? configJsonObject["maxBurstOfPhotosynthesisEnergy"].toInt()
          : cellControllerParams.maxBurstOfPhotosynthesisEnergy;
  cellControllerParams.summerDaytimeToWholeDayRatio =
      configJsonObject.contains("summerDaytimeToWholeDayRatio")
          ? static_cast<float>(configJsonObject["summerDaytimeToWholeDayRatio"].toDouble())
          : cellControllerParams.summerDaytimeToWholeDayRatio;
  cellControllerParams.maxMinerals        = configJsonObject.contains("maxMinerals")
                                                ? configJsonObject["maxMinerals"].toInt()
                                                : cellControllerParams.maxMinerals;
  cellControllerParams.maxBurstOfMinerals = configJsonObject.contains("maxBurstOfMinerals")
                                                ? configJsonObject["maxBurstOfMinerals"].toInt()
                                                : cellControllerParams.maxBurstOfMinerals;
  cellControllerParams.energyPerMineral =
      configJsonObject.contains("energyPerMineral")
          ? static_cast<float>(configJsonObject["energyPerMineral"].toDouble())
          : cellControllerParams.energyPerMineral;
  cellControllerParams.maxBurstOfFoodEnergy = configJsonObject.contains("maxBurstOfFoodEnergy")
                                                  ? configJsonObject["maxBurstOfFoodEnergy"].toInt()
                                                  : cellControllerParams.maxBurstOfFoodEnergy;
  cellControllerParams.childBudMutationChance =
      configJsonObject.contains("childBudMutationChance")
          ? static_cast<float>(configJsonObject["childBudMutationChance"].toDouble())
          : cellControllerParams.childBudMutationChance;
  cellControllerParams.parentBudMutationChance =
      configJsonObject.contains("parentBudMutationChance")
          ? static_cast<float>(configJsonObject["parentBudMutationChance"].toDouble())
          : cellControllerParams.parentBudMutationChance;
  cellControllerParams.randomMutationChance =
      configJsonObject.contains("randomMutationChance")
          ? static_cast<float>(configJsonObject["randomMutationChance"].toDouble())
          : cellControllerParams.randomMutationChance;
  cellControllerParams.dayDurationInTicks   = configJsonObject.contains("dayDurationInTicks")
                                                  ? configJsonObject["dayDurationInTicks"].toInt()
                                                  : cellControllerParams.dayDurationInTicks;
  cellControllerParams.seasonDurationInDays = configJsonObject.contains("seasonDurationInDays")
                                                  ? configJsonObject["seasonDurationInDays"].toInt()
                                                  : cellControllerParams.seasonDurationInDays;
  cellControllerParams.gammaFlashPeriodInDays =
      configJsonObject.contains("gammaFlashPeriodInDays")
          ? configJsonObject["gammaFlashPeriodInDays"].toInt()
          : cellControllerParams.gammaFlashPeriodInDays;
  cellControllerParams.gammaFlashMaxMutationsCount =
      configJsonObject.contains("gammaFlashMaxMutationsCount")
          ? configJsonObject["gammaFlashMaxMutationsCount"].toInt()
          : cellControllerParams.gammaFlashMaxMutationsCount;
  cellControllerParams.enableInstructionTurn =
      configJsonObject.contains("enableInstructionTurn")
          ? configJsonObject["enableInstructionTurn"].toBool()
          : cellControllerParams.enableInstructionTurn;
  cellControllerParams.enableInstructionMove =
      configJsonObject.contains("enableInstructionMove")
          ? configJsonObject["enableInstructionMove"].toBool()
          : cellControllerParams.enableInstructionMove;
  cellControllerParams.enableInstructionGetEnergyFromPhotosynthesis =
      configJsonObject.contains("enableInstructionGetEnergyFromPhotosynthesis")
          ? configJsonObject["enableInstructionGetEnergyFromPhotosynthesis"].toBool()
          : cellControllerParams.enableInstructionGetEnergyFromPhotosynthesis;
  cellControllerParams.enableInstructionGetEnergyFromMinerals =
      configJsonObject.contains("enableInstructionGetEnergyFromMinerals")
          ? configJsonObject["enableInstructionGetEnergyFromMinerals"].toBool()
          : cellControllerParams.enableInstructionGetEnergyFromMinerals;
  cellControllerParams.enableInstructionGetEnergyFromFood =
      configJsonObject.contains("enableInstructionGetEnergyFromFood")
          ? configJsonObject["enableInstructionGetEnergyFromFood"].toBool()
          : cellControllerParams.enableInstructionGetEnergyFromFood;
  cellControllerParams.enableInstructionBud =
      configJsonObject.contains("enableInstructionBud")
          ? configJsonObject["enableInstructionBud"].toBool()
          : cellControllerParams.enableInstructionBud;
  cellControllerParams.enableInstructionMutateRandomGene =
      configJsonObject.contains("enableInstructionMutateRandomGene")
          ? configJsonObject["enableInstructionMutateRandomGene"].toBool()
          : cellControllerParams.enableInstructionMutateRandomGene;
  cellControllerParams.enableInstructionShareEnergy =
      configJsonObject.contains("enableInstructionShareEnergy")
          ? configJsonObject["enableInstructionShareEnergy"].toBool()
          : cellControllerParams.enableInstructionShareEnergy;
  cellControllerParams.enableInstructionTouch =
      configJsonObject.contains("enableInstructionTouch")
          ? configJsonObject["enableInstructionTouch"].toBool()
          : cellControllerParams.enableInstructionTouch;
  cellControllerParams.enableInstructionDetermineEnergyLevel =
      configJsonObject.contains("enableInstructionDetermineEnergyLevel")
          ? configJsonObject["enableInstructionDetermineEnergyLevel"].toBool()
          : cellControllerParams.enableInstructionDetermineEnergyLevel;
  cellControllerParams.enableInstructionDetermineDepth =
      configJsonObject.contains("enableInstructionDetermineDepth")
          ? configJsonObject["enableInstructionDetermineDepth"].toBool()
          : cellControllerParams.enableInstructionDetermineDepth;
  cellControllerParams.enableInstructionDetermineBurstOfPhotosynthesisEnergy =
      configJsonObject.contains("enableInstructionDetermineBurstOfPhotosynthesisEnergy")
          ? configJsonObject["enableInstructionDetermineBurstOfPhotosynthesisEnergy"].toBool()
          : cellControllerParams.enableInstructionDetermineBurstOfPhotosynthesisEnergy;
  cellControllerParams.enableInstructionDetermineBurstOfMinerals =
      configJsonObject.contains("enableInstructionDetermineBurstOfMinerals")
          ? configJsonObject["enableInstructionDetermineBurstOfMinerals"].toBool()
          : cellControllerParams.enableInstructionDetermineBurstOfMinerals;
  cellControllerParams.enableInstructionDetermineBurstOfMineralEnergy =
      configJsonObject.contains("enableInstructionDetermineBurstOfMineralEnergy")
          ? configJsonObject["enableInstructionDetermineBurstOfMineralEnergy"].toBool()
          : cellControllerParams.enableInstructionDetermineBurstOfMineralEnergy;
  cellControllerParams.enableZeroEnergyOrganic =
      configJsonObject.contains("enableZeroEnergyOrganic")
          ? configJsonObject["enableZeroEnergyOrganic"].toBool()
          : cellControllerParams.enableZeroEnergyOrganic;
  cellControllerParams.enableForcedBuddingOnMaximalEnergyLevel =
      configJsonObject.contains("enableForcedBuddingOnMaximalEnergyLevel")
          ? configJsonObject["enableForcedBuddingOnMaximalEnergyLevel"].toBool()
          : cellControllerParams.enableForcedBuddingOnMaximalEnergyLevel;
  cellControllerParams.enableTryingToBudInUnoccupiedDirection =
      configJsonObject.contains("enableTryingToBudInUnoccupiedDirection")
          ? configJsonObject["enableTryingToBudInUnoccupiedDirection"].toBool()
          : cellControllerParams.enableTryingToBudInUnoccupiedDirection;
  cellControllerParams.enableDeathOnBuddingIfNotEnoughSpace =
      configJsonObject.contains("enableDeathOnBuddingIfNotEnoughSpace")
          ? configJsonObject["enableDeathOnBuddingIfNotEnoughSpace"].toBool()
          : cellControllerParams.enableDeathOnBuddingIfNotEnoughSpace;
  cellControllerParams.enableSeasons  = configJsonObject.contains("enableSeasons")
                                            ? configJsonObject["enableSeasons"].toBool()
                                            : cellControllerParams.enableSeasons;
  cellControllerParams.enableDaytimes = configJsonObject.contains("enableDaytimes")
                                            ? configJsonObject["enableDaytimes"].toBool()
                                            : cellControllerParams.enableDaytimes;
  cellControllerParams.enableMaximizingFoodEnergy =
      configJsonObject.contains("enableMaximizingFoodEnergy")
          ? configJsonObject["enableMaximizingFoodEnergy"].toBool()
          : cellControllerParams.enableMaximizingFoodEnergy;
  cellControllerParams.enableDeadCellPinningOnSinking =
      configJsonObject.contains("enableDeadCellPinningOnSinking")
          ? configJsonObject["enableDeadCellPinningOnSinking"].toBool()
          : cellControllerParams.enableDeadCellPinningOnSinking;
  if (configJsonObject.contains("firstCellGenome")) {
    QJsonArray       firstCellGenomeArray = configJsonObject["firstCellGenome"].toArray();
    std::vector<int> firstCellGenome;
    for (QJsonValueRef &&firstCellGenomeInstruction : firstCellGenomeArray) {
      firstCellGenome.push_back(firstCellGenomeInstruction.toInt());
    }
    cellControllerParams.firstCellGenome = firstCellGenome;
  }
  cellControllerParams.firstCellEnergyMultiplier =
      configJsonObject.contains("firstCellEnergyMultiplier")
          ? static_cast<float>(configJsonObject["firstCellEnergyMultiplier"].toDouble())
          : cellControllerParams.firstCellEnergyMultiplier;
  cellControllerParams.firstCellDirection = configJsonObject.contains("firstCellDirection")
                                                ? configJsonObject["firstCellDirection"].toInt()
                                                : cellControllerParams.firstCellDirection;
  cellControllerParams.firstCellIndexMultiplier =
      configJsonObject.contains("firstCellIndexMultiplier")
          ? static_cast<float>(configJsonObject["firstCellIndexMultiplier"].toDouble())
          : cellControllerParams.firstCellIndexMultiplier;

  return 0;
}

// Processes command line arguments updating CellController::Params and Controls
int processCommandLineArguments(int argc, char *argv[], const std::string &title,
                                Controls                          &controls,
                                cellarium::CellController::Params &cellControllerParams) {
  // Initializing and configuring QCoreApplication
  QCoreApplication qCoreApplication{argc, argv};
  QCoreApplication::setApplicationName(QString{title.c_str()});
  QCoreApplication::setApplicationVersion("1.0");

  // Initializing and configuring QCommandLineParser
  QCommandLineParser commandLineParser{};
  commandLineParser.setApplicationDescription(
      "cellarium is a graphical simulation of a discrete world inhabited by cells that exists "
      "according to the "
      "laws of the evolutionary algorithm.");
  commandLineParser.addHelpOption();
  commandLineParser.addPositionalArgument(
      "config", QCoreApplication::translate("main", "Path to configuration file."));
  commandLineParser.addOption(
      {QStringList() << "v"
                     << "version",
       QCoreApplication::translate("main", "Shows application version and license.")});
  commandLineParser.addOption({QStringList() << "c"
                                             << "controls",
                               QCoreApplication::translate("main", "Shows controls help.")});
  commandLineParser.addOption(
      {QStringList() << "g"
                     << "generate",
       QCoreApplication::translate("main", "Generates default configuration file.")});

  // Processing QCommandLineParser and getting positional arguments
  commandLineParser.process(qCoreApplication);
  const QStringList positionalArgumentList{commandLineParser.positionalArguments()};

  // If version and license requested
  if (commandLineParser.isSet("version")) {
    outputVersionAndLicense();
    return -1;
  }

  // If controls help requested
  if (commandLineParser.isSet("controls")) {
    outputControlsHelp();
    return -1;
  }

  // If config file generation requested
  if (commandLineParser.isSet("generate")) {
    generateDefaultConfigurationFile();
    return -1;
  }

  // If no config file specified
  if (positionalArgumentList.size() < 1) {
    std::cout << "No config file specified. See help with -h or --help." << std::endl;
    return -1;
  }

  // Loading configuration file
  return loadConfigurationFile(positionalArgumentList.at(0), controls, cellControllerParams);
}
