// STL libs
#include <windows.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <unordered_map>

// Project libs
#include "include/config.h"
#include "include/pdn_container.h"

#define __PROJECT_VERSION__ "v0.0.1"
#define __DEFAULT_METHODS_STEP__ 0.01
#define __METHODS_STEP_CHANGE_BY__ 0.001
#define __BOTTOM_BORDER__ 0.9

int main(int args, const char* argv[])
{
    Config config(args, argv);

    if (!config.isHelp) {

        std::cout << "C++ Standard 20\nSpice Fake Generator " << __PROJECT_VERSION__
                  << "\n================================================================================\n\n"
                  << std::flush;

        try {
            Value irDropDiffStep = (config.irDropDiff) / config.numOfFakes;
            uint64_t sumTimeOfGeneration {};
            Value sumOfPercentageDifferences {};
            std::array<Value, 3> sumOfFakeIRDrops {};

            PDNContainer pdnContainer(config.source);

            pdnContainer.solveDCAndSaveRealValues(config.irDropPrecision, config.maxIterations);

            for (size_t i = 0; i < config.numOfFakes; ++i) {
                auto start = std::chrono::high_resolution_clock::now();
                auto min = std::fabs((irDropDiffStep * __BOTTOM_BORDER__) * (i + 1));
                bool positive = static_cast<bool>(irDropDiffStep > 0);
                bool negative = static_cast<bool>(irDropDiffStep < 0);
                uint32_t totalSteps {};
                uint32_t totalIterations {};
                Value meanDifference {};
                Value methodsStep = __DEFAULT_METHODS_STEP__ * irDropDiffStep;
                std::array<Value, 3> fakeIRDrops {};

                std::cout << "\nCreating: netlist-fake-" << i << "\n"
                          << std::flush;

                do {
                    switch (config.mode) {
                    case 1:
                        pdnContainer.inverseCurrentSourcesPositions(methodsStep);
                        break;
                    case 2:
                        pdnContainer.connectFakeCurrentSources(methodsStep);
                        break;
                    case 3:
                        pdnContainer.changeCurrentSourceValue(methodsStep);
                        break;
                    default:
                        break;
                    }

                    totalIterations += pdnContainer.solveDC(config.irDropPrecision, config.maxIterations);
                    meanDifference = pdnContainer.compareFakeWithRealValues();
                    fakeIRDrops = pdnContainer.calculateIRDrop();

                    std::cout << "Step: " << ++totalSteps
                              << " -- Total iterations: " << totalIterations
                              << " -- IR-Drop difference: " << meanDifference * 100.0 << "%"
                              << " -- Max: " << fakeIRDrops[0]
                              << " -- Min: " << fakeIRDrops[1]
                              << " -- Mean: " << fakeIRDrops[2] << "\r" << std::flush;

                    if (meanDifference >= min) {
                        std::cout << std::endl;
                        break;
                    }

                    methodsStep -= methodsStep * __METHODS_STEP_CHANGE_BY__;
                } while (true);

                sumOfFakeIRDrops[0] += fakeIRDrops[0];
                sumOfFakeIRDrops[1] += fakeIRDrops[1];
                sumOfFakeIRDrops[2] += fakeIRDrops[2];

                if (!std::filesystem::exists(config.destination))
                    std::filesystem::create_directories(config.destination);

                std::ostringstream fakeFolderName;
                fakeFolderName << config.destination + "/netlist-fake-"
                               << "mode-" << static_cast<uint32_t>(config.mode) << "-" << i;
                std::filesystem::create_directory(fakeFolderName.str());

                std::ostringstream spiceFileName;
                spiceFileName << fakeFolderName.str() + "/netlist.sp";
                pdnContainer.writeNetlistToFile(spiceFileName.str());

                std::ostringstream irdropFileName;
                irdropFileName << fakeFolderName.str() + "/netlist.csv";
                pdnContainer.writeIRDropToFile(irdropFileName.str());

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                sumTimeOfGeneration += duration;
                sumOfPercentageDifferences += meanDifference;
            }

            std::cout << "\nIR-Drop statistics:\n\n";
            std::cout << std::fixed << std::setprecision(9);
            std::cout << "Mean -- Max: " << sumOfFakeIRDrops[0] / config.numOfFakes << "\n";
            std::cout << "Mean -- Min: " << sumOfFakeIRDrops[1] / config.numOfFakes << "\n";
            std::cout << "Mean -- Mean: " << sumOfFakeIRDrops[2] / config.numOfFakes << "\n";
            std::cout << "Mean -- MeanDiff: " << sumOfPercentageDifferences / config.numOfFakes * 100.0 << "\n";

            if (sumTimeOfGeneration != 0) {
                std::cout << "\nTotal time of generation: " << sumTimeOfGeneration << " ms\n";
                std::cout << "Average time of generation: " << sumTimeOfGeneration / config.numOfFakes << " ms\n";
            }
        } catch (std::invalid_argument& e) {
            std::cerr << "\nArgument error: " << e.what() << "\n";
        }

        std::cout
            << "\n================================================================================\nEnd of generating.\n"
            << std::flush;
    }

    return 0;
}
