// STL Libs
#include <iomanip>
#include <sstream>

// Project libs
#include "../include/current_source.h"

CurrentSource::CurrentSource(const NodeCoords& t_firstNode, const Value& t_value, const Name& t_name)
    : m_layer((uint8_t)t_firstNode[0])
    , m_x(t_firstNode[1])
    , m_y(t_firstNode[2])
    , value(t_value)
    , name(t_name) {};

void CurrentSource::setNewCoords(const NodeCoords& t_node)
{
    m_layer = (uint8_t)t_node[0];
    m_x = t_node[1];
    m_y = t_node[2];
}

std::string CurrentSource::toString()
{
    std::stringstream withoutNameDescription;
    withoutNameDescription << "n1_m" << (uint32_t)(m_layer) << "_" << m_x << "_" << m_y << " "
                           << "0"
                           << " " << std::fixed << std::setprecision(9) << value;

    return withoutNameDescription.str();
}

void CurrentSource::fullyDisconnection()
{
    connectedNode = nullptr;
};