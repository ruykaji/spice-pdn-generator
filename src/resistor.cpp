// STL libs
#include <iomanip>
#include <sstream>

// Project libs
#include "../include/resistor.h"

Resistor::Resistor(const NodeCoords& t_firstNode, const NodeCoords& t_secondNode, const Value& t_value,
    const Name& t_name)
    : m_layer({ (uint8_t)t_firstNode[0], (uint8_t)t_secondNode[0] })
    , m_x({ t_firstNode[1], t_secondNode[1] })
    , m_y({ t_firstNode[2], t_secondNode[2] })
    , value(t_value)
    , name(t_name) {};

std::string Resistor::toString()
{
    std::stringstream withoutNameDescription;
    withoutNameDescription << "n1_m" << (uint32_t)(m_layer[0]) << "_" << m_x[0] << "_" << m_y[0] << " "
                           << "n1_m" << (uint32_t)(m_layer[1]) << "_" << m_x[1] << "_" << m_y[1] << " " << std::fixed
                           << std::setprecision(9) << value;

    return withoutNameDescription.str();
}

bool Resistor::isViaResistor()
{
    if (m_layer[0] != m_layer[1])
        return true;
    return false;
}

void Resistor::fullyDisconnection()
{
    connectedNodes.clear();
};