#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    bool isHelp {};
    uint8_t mode { 1 };
    uint16_t numOfFakes { 10 };
    uint32_t maxIterations { 100000 };
    float irDropDiff { 0.75 };
    double irDropPrecision { 1e-8 };
    std::string source { "./netlist.sp" };
    std::string destination { "./" };

    Config(const int& args, const char* argv[]);
};

#endif