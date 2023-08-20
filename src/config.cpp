#include <iostream>

#include "../include/config.h"

Config::Config(const int& args, const char* argv[])
{
    for (size_t i = 1; i < args; ++i) {
        if (std::string(argv[i]) == "--source" || std::string(argv[i]) == "-s") {
            source = argv[i + 1];
        } else if (std::string(argv[i]) == "--destination" || std::string(argv[i]) == "-d") {
            destination = argv[i + 1];
        } else if (std::string(argv[i]) == "--mode" || std::string(argv[i]) == "-m") {
            mode = std::stol(argv[i + 1]);
        } else if (std::string(argv[i]) == "--irDropPrecision" || std::string(argv[i]) == "-irp") {
            irDropPrecision = std::stod(argv[i + 1]);
        } else if (std::string(argv[i]) == "--maxIterations" || std::string(argv[i]) == "-mi") {
            maxIterations = std::stol(argv[i + 1]);
        } else if (std::string(argv[i]) == "--irDropDiff" || std::string(argv[i]) == "-ird") {
            irDropDiff = std::stod(argv[i + 1]);
        } else if (std::string(argv[i]) == "--numOfFakes" || std::string(argv[i]) == "-nof") {
            numOfFakes = std::stol(argv[i + 1]);
        } else if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            std::cout << "Usage: \n"
                      << "--help [-h] - Show help information.\n\n"
                      << "--source [-s] - Path to source .sp file. Default - ./netlist.sp\n\n"
                      << "--destination [-d] - Path to destination folder where will be stored all generated fakes. Default - ./\n\n"
                      << "--mode [-m] - Mode of generator: '1' - Moves current sources from origin nodes to random ones. '2' - Add new current sources to random nodes. '3' - Increase volume of current sources. Default - 1\n\n"
                      << "--irDropPrecision [-irp] - Precision of ir-drop calculation. Default - 1e-8\n\n"
                      << "--maxIterations [-mi] - Maximum number of iterations of ir-drop calculation. Default - 100000\n\n"
                      << "--irDropDiff [-ird] - Expected max difference of fake's pdn's ir-drop values from original pdn. Default - 0.75\n\n"
                      << "--numOfFakes [-nof] - Numb of fake pdn to generate. Default - 10\n\n" << std::flush;
        };
    }
}