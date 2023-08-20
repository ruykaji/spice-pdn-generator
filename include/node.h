#ifndef NODE_H
#define NODE_H

// STL Libs
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Types
#include "types.h"

class Node {
public:
    bool isVoltageNode {};
    bool isAbelToConnectVoltageSource {};
    bool isAbelToConnectCurrentSource {};
    bool isFakedByCurrentSource {};
    Value realValue {};
    Value value {};
    Name name {};
    NodePtrVec neighborNodes {};

private:
    Value m_sumOfCurrent {};
    Value m_inverseSumOfResistance {};
    L m_layer {};
    X m_x {};
    Y m_y {};
    ResistorPtrVec m_connectedResistors {};
    CurrentSourcePtrVec m_connectedCurrentSources {};
    VoltageSourcePtrVec m_connectedVoltageSources {};

public:
    Node() = default;
    ~Node() = default;
    Node(const NodeCoords& t_node, const Name& t_name);

    /**
     * @brief Add a inverse resistor value to this node.
     *
     * @param t_resistance resistance value to be added
     */
    void addInverseSumOfResistance(const Value& t_resistance);

    /**
     * @brief Gets the node coordinates.
     *
     * @return NodeData - node coordinates.
     */
    NodeCoords getCoordinates();

    /**
     * @brief Connects a resistor to this node.
     *
     * @param t_resistor resistor to be connected to this node.
     */
    void connectResistor(const ResistorPtr& t_resistor);

    /**
     * @brief Connects a current source to this node.
     *
     * @param t_currentSource current source to be connected to this node.
     */
    void connectCurrentSource(const CurrentSourcePtr& t_currentSource);

    /**
     * @brief Connects a voltage source to this node.
     *
     * @param t_voltageSource voltage source to be connected to this node.
     */
    void connectVoltageSource(const VoltageSourcePtr& t_voltageSource);

    /**
     * @brief Disconnects a current source from this node.
     *
     * @param t_currentSource current source to be disconnected from this node.
     */
    void disconnectCurrentSource(const CurrentSourcePtr& t_currentSource);

    /**
     * @brief Disconnects a voltage source from this node.
     *
     * @param t_voltageSource voltage source to be disconnected from this node.
     */
    void disconnectVoltageSource(const VoltageSourcePtr& t_voltageSource);

    /**
     * @brief Initializes the node before dc ir-drop.
     *
     */
    void initializationDC();

    /**
     * @brief Reinitializes the node without recalculation its default value.
     *
     */
    void reinitializationDC();

    /**
     * @brief Makes step for the node in calculation dc ir-drop.
     *
     */
    int stepDC(const Value& t_precision);

    /**
     * @brief Checks if this node is connected to vias.
     *
     * @return true - this node is connected to vias.
     * @return false - this node is not connected to vias.
     */
    bool isViaNode();

    /**
     * @brief Disconnects the node from all other elements.
     *
     */
    void fullyDisconnection();
};

#endif