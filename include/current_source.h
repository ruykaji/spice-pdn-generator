#ifndef CURRENT_SOURCE_H
#define CURRENT_SOURCE_H

// STL Libs
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Types
#include "types.h"

class CurrentSource {
public:
    Value value {};
    Name name {};
    NodePtr connectedNode;

private:
    L m_layer {};
    X m_x {};
    Y m_y {};

public:
    CurrentSource() = default;
    ~CurrentSource() = default;
    CurrentSource(const NodeCoords& t_fistNode, const Value& t_value, const Name& t_name);

    void setNewCoords(const NodeCoords& t_node);

    /**
     * @brief Creates line description of the current source in spice format.
     *
     * @return std::string - line description.
     */
    std::string toString();

    /**
     * @brief Disconnects the current source from all other elements.
     *
     */
    void fullyDisconnection();
};

#endif