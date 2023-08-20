#ifndef VOLTAGE_SOURCE_H
#define VOLTAGE_SOURCE_H

// STL Libs
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Types
#include "types.h"

class VoltageSource {
public:
    Value value {};
    Name name {};
    NodePtr connectedNode;

private:
    L m_layer {};
    X m_x {};
    Y m_y {};

public:
    VoltageSource() = default;
    ~VoltageSource() = default;
    VoltageSource(const NodeCoords& t_fistNode, const Value& t_value, const Name& t_name);

    /**
     * @brief Creates line description of the voltage source in spice format.
     *
     * @return std::string - line description.
     */
    std::string toString();

    /**
     * @brief Disconnects the voltage source from all other elements.
     *
     */
    void fullyDisconnection();
};

#endif