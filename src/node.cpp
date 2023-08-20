// STL libs
#include <algorithm>
#include <cmath>

// Project libs
#include "../include/current_source.h"
#include "../include/node.h"
#include "../include/resistor.h"
#include "../include/voltage_source.h"

Node::Node(const NodeCoords& t_node, const Name& t_name)
    : m_layer((uint8_t)t_node[0])
    , m_x(t_node[1])
    , m_y(t_node[2])
    , name(t_name) {};

void Node::addInverseSumOfResistance(const Value& t_resistance)
{
    m_inverseSumOfResistance += 1.0 / t_resistance;
}

NodeCoords Node::getCoordinates()
{
    return NodeCoords({ m_layer, m_x, m_y });
};

void Node::connectResistor(const ResistorPtr& t_resistor)
{
    m_connectedResistors.push_back(t_resistor);
};

void Node::connectCurrentSource(const CurrentSourcePtr& t_currentSource)
{
    m_connectedCurrentSources.push_back(t_currentSource);
};

void Node::connectVoltageSource(const VoltageSourcePtr& t_voltageSource)
{
    m_connectedVoltageSources.push_back(t_voltageSource);
};

void Node::disconnectCurrentSource(const CurrentSourcePtr& t_currentSource)
{
    auto targetCurrentSourceName = t_currentSource->name;

    for (auto it = m_connectedCurrentSources.begin(); it != m_connectedCurrentSources.end();) {
        auto currentSourceName = (*it)->name;

        if (currentSourceName == targetCurrentSourceName) {
            m_connectedCurrentSources.erase(it);
            break;
        }

        ++it;
    }
}

void Node::disconnectVoltageSource(const VoltageSourcePtr& t_voltageSource)
{
    auto targetVoltageSourceName = t_voltageSource->name;

    for (auto it = m_connectedVoltageSources.begin(); it != m_connectedVoltageSources.end();) {
        auto voltageSourceName = (*it)->name;

        if (voltageSourceName == targetVoltageSourceName) {
            m_connectedVoltageSources.erase(it);
            break;
        }

        ++it;
    }
}

bool Node::isViaNode()
{
    for (const auto& resistor : m_connectedResistors) {
        if (resistor->isViaResistor()) {
            return true;
        }
    }

    return false;
}

void Node::initializationDC()
{
    if (isVoltageNode)
        return;

    Value sumOfNodes {};

    for (uint16_t i {}; i < m_connectedResistors.size(); ++i)
        sumOfNodes += neighborNodes[i]->value / m_connectedResistors[i]->value;

    for (auto& currentSource : m_connectedCurrentSources)
        m_sumOfCurrent += currentSource->value;

    value = (m_sumOfCurrent - sumOfNodes) / m_inverseSumOfResistance;
}

void Node::reinitializationDC()
{
    if (isVoltageNode)
        return;

    m_sumOfCurrent = 0;

    for (auto& currentSource : m_connectedCurrentSources)
        m_sumOfCurrent += currentSource->value;
}

int Node::stepDC(const Value& t_precision)
{
    if (isVoltageNode)
        return 1;

    Value sumOfNodes {};
    Value previousValue = value;

    for (uint8_t i {}; i < m_connectedResistors.size(); ++i) {
        sumOfNodes += neighborNodes[i]->value / m_connectedResistors[i]->value;
    }

    value = (sumOfNodes - m_sumOfCurrent) / m_inverseSumOfResistance;

    if (std::fabs(value - previousValue) < t_precision)
        return 1;

    return 0;
}

void Node::fullyDisconnection()
{
    m_connectedResistors.clear();
    m_connectedCurrentSources.clear();
    m_connectedVoltageSources.clear();
    neighborNodes.clear();
};