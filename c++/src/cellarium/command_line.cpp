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
#include "./command_line.hpp"

// STD
#include <iostream>

// Qt
#include <QByteArray>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QStringList>

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
                          CellEvolution::kInitPhotosynthesisDepthMultiplier);
  configJsonObject.insert("maxMineralHeightMultiplier",
                          CellEvolution::kInitMineralHeightMultiplier);
  configJsonObject.insert("randomSeed", static_cast<int>(CellEvolution::kInitRandomSeed));
  configJsonObject.insert("width", CellEvolution::kInitWidth);
  configJsonObject.insert("height", CellEvolution::kInitHeight);
  configJsonObject.insert("cellSize", CellEvolution::kInitCellSize);
  configJsonObject.insert("genomSize", CellEvolution::kInitGenomSize);
  configJsonObject.insert("maxInstructionsPerTick", CellEvolution::kInitMaxInstructionsPerTick);
  configJsonObject.insert("maxAkinGenomDifference", CellEvolution::kInitMaxAkinGenomDifference);
  configJsonObject.insert("minChildEnergy", CellEvolution::kInitMinChildEnergy);
  configJsonObject.insert("maxEnergy", CellEvolution::kInitMaxEnergy);
  configJsonObject.insert("maxBurstOfPhotosynthesisEnergy",
                          CellEvolution::kInitMaxBurstOfPhotosynthesisEnergy);
  configJsonObject.insert("summerDaytimeToWholeDayRatio",
                          CellEvolution::kInitSummerDaytimeToWholeDayRatio);
  configJsonObject.insert("maxMinerals", CellEvolution::kInitMaxMinerals);
  configJsonObject.insert("maxBurstOfMinerals", CellEvolution::kInitMaxBurstOfMinerals);
  configJsonObject.insert("energyPerMineral", CellEvolution::kInitEnergyPerMineral);
  configJsonObject.insert("maxBurstOfFoodEnergy", CellEvolution::kInitMaxBurstOfFoodEnergy);
  configJsonObject.insert("randomMutationChance", CellEvolution::kInitRandomMutationChance);
  configJsonObject.insert("budMutationChance", CellEvolution::kInitBudMutationChance);
  configJsonObject.insert("dayDurationInTicks", CellEvolution::kInitDayDurationInTicks);
  configJsonObject.insert("seasonDurationInDays", CellEvolution::kInitSeasonDurationInDays);
  configJsonObject.insert("gammaFlashPeriodInDays", CellEvolution::kInitGammaFlashPeriodInDays);
  configJsonObject.insert("gammaFlashMaxMutationsCount",
                          CellEvolution::kInitGammaFlashMaxMutationsCount);
  configJsonObject.insert("enableInstructionTurn", CellEvolution::kInitEnableInstructionTurn);
  configJsonObject.insert("enableInstructionMove", CellEvolution::kInitEnableInstructionMove);
  configJsonObject.insert("enableInstructionGetEnergyFromPhotosynthesis",
                          CellEvolution::kInitEnableInstructionGetEnergyFromPhotosynthesis);
  configJsonObject.insert("enableInstructionGetEnergyFromMinerals",
                          CellEvolution::kInitEnableInstructionGetEnergyFromMinerals);
  configJsonObject.insert("enableInstructionGetEnergyFromFood",
                          CellEvolution::kInitEnableInstructionGetEnergyFromFood);
  configJsonObject.insert("enableInstructionBud", CellEvolution::kInitEnableInstructionBud);
  configJsonObject.insert("enableInstructionMutateRandomGen",
                          CellEvolution::kInitEnableInstructionMutateRandomGen);
  configJsonObject.insert("enableInstructionShareEnergy",
                          CellEvolution::kInitEnableInstructionShareEnergy);
  configJsonObject.insert("enableInstructionTouch", CellEvolution::kInitEnableInstructionTouch);
  configJsonObject.insert("enableInstructionDetermineEnergyLevel",
                          CellEvolution::kInitEnableInstructionDetermineEnergyLevel);
  configJsonObject.insert("enableInstructionDetermineDepth",
                          CellEvolution::kInitEnableInstructionDetermineDepth);
  configJsonObject.insert(
      "enableInstructionDetermineBurstOfPhotosynthesisEnergy",
      CellEvolution::kInitenableInstructionDetermineBurstOfPhotosynthesisEnergy);
  configJsonObject.insert("enableInstructionDetermineBurstOfMinerals",
                          CellEvolution::kInitEnableInstructionDetermineBurstOfMinerals);
  configJsonObject.insert("enableInstructionDetermineBurstOfMineralEnergy",
                          CellEvolution::kInitEnableInstructionDetermineBurstOfMineralEnergy);
  configJsonObject.insert("enableZeroEnergyOrganic", CellEvolution::kInitEnableZeroEnergyOrganic);
  configJsonObject.insert("enableForcedBuddingOnMaximalEnergyLevel",
                          CellEvolution::kInitEnableForcedBuddingOnMaximalEnergyLevel);
  configJsonObject.insert("enableTryingToBudInUnoccupiedDirection",
                          CellEvolution::kInitEnableTryingToBudInUnoccupiedDirection);
  configJsonObject.insert("enableDeathOnBuddingIfNotEnoughSpace",
                          CellEvolution::kInitEnableDeathOnBuddingIfNotEnoughSpace);
  configJsonObject.insert("enableSeasons", CellEvolution::kInitEnableSeasons);
  configJsonObject.insert("enableDaytimes", CellEvolution::kInitEnableDaytimes);
  configJsonObject.insert("enableMaximizingFoodEnergy",
                          CellEvolution::kInitEnableMaximizingFoodEnergy);
  configJsonObject.insert("enableDeadCellPinningOnSinking",
                          CellEvolution::kInitEnableDeadCellPinningOnSinking);

  // Writing configuration to file
  QJsonDocument configJsonDocument{configJsonObject};
  configFile.write(configJsonDocument.toJson());

  return 0;
}

// Processes command line arguments updating CellController::Params and Controls
int processCommandLineArguments(int argc, char *argv[], const std::string &title,
                                Controls                              &controls,
                                CellEvolution::CellController::Params &cellControllerParams) {
  // Initializing and configuring QCoreApplication
  QCoreApplication qCoreApplication{argc, argv};
  QCoreApplication::setApplicationName(QString{title.c_str()});
  QCoreApplication::setApplicationVersion("1.0");

  // Initializing and configuring QCommandLineParser
  QCommandLineParser commandLineParser{};
  commandLineParser.setApplicationDescription(
      "Graphical simulation of a discrete world inhabited by cells that exists according to the "
      "laws of the evolutionary algorithm.");
  commandLineParser.addHelpOption();
  commandLineParser.addVersionOption();
  commandLineParser.addPositionalArgument(
      "config", QCoreApplication::translate("main", "Path to configuration file."));
  commandLineParser.addOption(
      {QStringList() << "g"
                     << "generate",
       QCoreApplication::translate("main", "Generates default configuration file.")});

  // Processing QCommandLineParser and getting positional arguments
  commandLineParser.process(qCoreApplication);
  const QStringList positionalArgumentList{commandLineParser.positionalArguments()};

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

  // Opening config file
  QFile configFile{positionalArgumentList.at(0)};
  if (!configFile.open(QFile::ReadOnly | QFile::Text)) {
    std::cout << "error: cannot open " << positionalArgumentList.at(0).toStdString() << std::endl;
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
  cellControllerParams.genomSize = configJsonObject.contains("genomSize")
                                       ? configJsonObject["genomSize"].toInt()
                                       : cellControllerParams.genomSize;
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
  cellControllerParams.randomMutationChance =
      configJsonObject.contains("randomMutationChance")
          ? static_cast<float>(configJsonObject["randomMutationChance"].toDouble())
          : cellControllerParams.randomMutationChance;
  cellControllerParams.budMutationChance =
      configJsonObject.contains("budMutationChance")
          ? static_cast<float>(configJsonObject["budMutationChance"].toDouble())
          : cellControllerParams.budMutationChance;
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
  cellControllerParams.enableInstructionMutateRandomGen =
      configJsonObject.contains("enableInstructionMutateRandomGen")
          ? configJsonObject["enableInstructionMutateRandomGen"].toBool()
          : cellControllerParams.enableInstructionMutateRandomGen;
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

  return 0;
}
