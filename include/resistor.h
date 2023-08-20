#ifndef RESISTOR_H
#define RESISTOR_H

// STL Libs
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Types
#include "types.h"

class Resistor {
public:
    Value value {};
    Name name {};
    NodePtrVec connectedNodes {};

private:
    std::array<L, 2> m_layer {};
    std::array<X, 2> m_x {};
    std::array<Y, 2> m_y {};

public:
    Resistor() = default;
    ~Resistor() = default;
    Resistor(const NodeCoords& t_fistNode, const NodeCoords& t_secondNode, const Value& t_value,
        const Name& t_name);

    /**
     * @brief Checks if the resistor is vias;
     *
     * @return true - resistor is vias;
     * @return false - resistor is not vias.
     */
    bool isViaResistor();

    /**
     * @brief Creates line description of the resistor in spice format.
     *
     * @return std::string - line description.
     */
    std::string toString();

    /**
     * @brief Disconnects the resistor from all other elements.
     *
     */
    void fullyDisconnection();
};

#endif