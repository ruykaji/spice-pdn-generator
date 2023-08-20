#ifndef PDN_CONTAINER_H
#define PDN_CONTAINER_H

// STL Libs
#include <fstream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// Types
#include "types.h"

class PDNContainer {
    Value m_voltageSourceValue {};
    std::vector<std::string> m_file {};
    std::vector<std::shared_ptr<Node>> m_nodes {};
    std::vector<std::shared_ptr<Resistor>> m_resistors {};
    std::vector<std::shared_ptr<VoltageSource>> m_voltageSources {};
    std::vector<std::shared_ptr<CurrentSource>> m_currentSources {};
    std::mt19937 m_generator {};

public:
    PDNContainer() = default;
    PDNContainer(const std::string& t_fileName);
    ~PDNContainer();

private:
    // =================================================================
    // Utility methods

    /**
     * @brief Pars node name in coordinates and metal layer. Name format: n[net]_m[layer]_[x]_[y].
     *
     * @param t_nodeName name of the node.
     * @return NodeData - std::array<uint32_t, 3> - coordinates and metal layer.
     */
    NodeCoords parseNodeName(const std::string& t_nodeName);

public:
    // =================================================================
    // PDN methods

    /**
     * @brief Disconnects all elements from each others.
     *
     */
    void fullyDisconnection();

    /**
     * @brief Resets the pnd graph to initial state.
     *
     */
    void resetWorkingGraph();

    // =================================================================
    // Change current sources

    /**
     * @brief Changes current sources values by multiplying by t_value.
     *
     * @param t_value value to multiply current sources by.
     */
    void changeCurrentSourceValue(const Value& t_value);

    /**
     * @brief Connects int(t_percentageDisconnect*size) current sources.
     *
     * @param t_percentageConnected percentage of current sources to connect.
     */
    void connectFakeCurrentSources(const Value& t_percentageConnect);
    
    /**
     * @brief Move current sources from original position to new one.
     * 
     * @param t_percentage percentage of current sources to move.
     */
    void inverseCurrentSourcesPositions(const Value& t_percentage);

    // =================================================================
    // IR-drop methods

    /**
     * @brief Solves the ir-drop for current pdn.
     *
     */
    uint64_t solveDC(const Value& t_precision, const uint64_t& t_maxIterations);

    /**
     * @brief Saves real values of real pdn nodes.
     *
     */
    void solveDCAndSaveRealValues(const Value& t_precision, const uint64_t& t_maxIterations);

    /**
     * @brief Compares real node values with fake node values.
     *
     */
    Value compareFakeWithRealValues();

    /**
     * @brief Calculates the ir-drop min, max and mean values for current pdn.
     *
     */
    std::array<Value, 3> calculateIRDrop();

    // =================================================================
    // Write/save methods

    /**
     * @brief Writes ir-drop solution of current pdn to a file.
     *
     * @param t_fileName path to the file to write to.
     */
    void writeIRDropToFile(const std::string& t_fileName);

    /**
     * @brief Writes the PDN container to a file in spice format.
     *
     * @param t_fileName path to the file to write to.
     */
    void writeNetlistToFile(const std::string& t_fileName);
};

#endif