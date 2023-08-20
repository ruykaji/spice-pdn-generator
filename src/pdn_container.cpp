// STL Libs
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <set>

// Project Libs
#include "../include/current_source.h"
#include "../include/node.h"
#include "../include/pdn_container.h"
#include "../include/resistor.h"
#include "../include/voltage_source.h"

// Expected size of the token
constexpr static uint8_t TOKEN_SIZE = 4;

/**
 * @brief Trim string from left and right by reference value
 *
 * @param s string to trim
 */
static inline void trim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !isspace(ch); }).base(), s.end());
};

/**
 * @brief Check if given character is element character
 *
 * @param c given character
 * @return true character is element character
 * @return false character is not element character
 */
static inline bool isElementLine(char& c)
{
    return c == 'i' || c == 'I' || c == 'R' || c == 'r' || c == 'V' || c == 'v';
}

PDNContainer::PDNContainer(const std::string& t_fileName)
{
    std::ifstream file(t_fileName);

    if (file.is_open()) {
        std::cout << "Parsing file - " << t_fileName << "\n"
                  << std::flush;

        std::string line {};
        uint64_t totalNodes {};
        uint64_t totalResistors {};
        uint64_t totalCurrentSources {};
        uint64_t totalVoltageSources {};

        while (std::getline(file, line)) {
            trim(line);

            if (isElementLine(line[0])) {
                m_file.push_back(line);
            }
        }

        resetWorkingGraph();

        std::cout << "\nFile information:\n\n"
                  << std::flush;
        std::cout << "- Total lines: " << m_file.size() << "\n"
                  << std::flush;
        std::cout << "- Total resistors: " << m_resistors.size() << "\n"
                  << std::flush;
        std::cout << "- Total current sources: " << m_currentSources.size() << "\n"
                  << std::flush;
        std::cout << "- Total voltage sources: " << m_voltageSources.size() << "\n"
                  << std::flush;
        std::cout << "- Total nodes: " << m_nodes.size() << "\n"
                  << std::flush;
    } else {
        std::cout << "Failed to open file - " << t_fileName << "\n"
                  << std::flush;
    }

    file.close();
};

PDNContainer::~PDNContainer(){
    fullyDisconnection();
}

// =================================================================
// Utility methods

NodeCoords PDNContainer::parseNodeName(const std::string& t_nodeName)
{
    if (t_nodeName != "0") {
        NodeCoords coordinates {};
        std::string numberStr {};
        bool isNumber = false;
        uint16_t i {};

        for (auto c = t_nodeName.begin(); c != t_nodeName.end(); ++c) {
            if (std::isdigit(*c)) {
                numberStr += *c;
                isNumber = true;
            } else {
                if (isNumber) {
                    if (i > 0)
                        coordinates[i - 1] = std::stoul(numberStr);

                    ++i;
                    isNumber = false;
                }

                numberStr.clear();
            }
        }

        if (isNumber && i > 0)
            coordinates[i - 1] = std::stoul(numberStr);

        return coordinates;
    }

    return { 0, 0, 0 };
}

// =================================================================
// PDN methods

void PDNContainer::fullyDisconnection()
{
    for (auto& node : m_nodes) {
        node->fullyDisconnection();
    }

    for (auto& resistor : m_resistors) {
        resistor->fullyDisconnection();
    }

    for (auto& currentSource : m_currentSources) {
        currentSource->fullyDisconnection();
    }

    for (auto& voltageSource : m_voltageSources) {
        voltageSource->fullyDisconnection();
    }

    m_nodes.clear();
    m_resistors.clear();
    m_currentSources.clear();
    m_voltageSources.clear();
}

void PDNContainer::resetWorkingGraph()
{
    // Temp map for faster access to elements while connecting.
    std::map<std::string, std::shared_ptr<Node>> nodes {};
    std::map<std::string, std::shared_ptr<Resistor>> resistors {};
    std::map<std::string, std::shared_ptr<VoltageSource>> voltageSources {};
    std::map<std::string, std::shared_ptr<CurrentSource>> currentSources {};

    // Initialize random number generator
    std::random_device rng {};
    m_generator = std::mt19937(rng());

    for (auto& line : m_file) {
        std::vector<std::string> lineTokens {};
        std::istringstream tokenStream(line);
        std::string lineToken {};

        while (std::getline(tokenStream, lineToken, ' '))
            lineTokens.push_back(lineToken);

        if (lineTokens.size() != TOKEN_SIZE)
            throw std::runtime_error(std::string("Invalid line: ") + line);

        NodeCoords firstNodeCoords = parseNodeName(lineTokens[1]);
        NodeCoords secondNodeCoords = parseNodeName(lineTokens[2]);
        Value value = std::stod(lineTokens[3]);

        switch (line[0]) {
        case 'r':
        case 'R': {
            auto resistor = std::make_shared<Resistor>(firstNodeCoords, secondNodeCoords, value, lineTokens[0]);

            m_resistors.push_back(resistor);
            resistors[lineTokens[0]] = ResistorPtr(resistor);

            if (!nodes.count(lineTokens[1])) {
                m_nodes.push_back(std::make_shared<Node>(firstNodeCoords, lineTokens[1]));
                nodes[lineTokens[1]] = NodePtr(m_nodes.back());
            }

            if (!nodes.count(lineTokens[2])) {
                m_nodes.push_back(std::make_shared<Node>(secondNodeCoords, lineTokens[2]));
                nodes[lineTokens[2]] = NodePtr(m_nodes.back());
            }

            auto firstNode = nodes[lineTokens[1]];
            auto secondNode = nodes[lineTokens[2]];

            resistor->connectedNodes.push_back(nodes[lineTokens[1]]);
            resistor->connectedNodes.push_back(nodes[lineTokens[2]]);

            firstNode->connectResistor(resistors[lineTokens[0]]);
            secondNode->connectResistor(resistors[lineTokens[0]]);

            // IR-drop preparations
            firstNode->neighborNodes.push_back(nodes[lineTokens[2]]);
            firstNode->addInverseSumOfResistance(resistor->value);

            if (firstNodeCoords[0] == 1)
                firstNode->isAbelToConnectCurrentSource = true;
            else if (firstNodeCoords[0] == 9)
                firstNode->isAbelToConnectVoltageSource = true;

            secondNode->neighborNodes.push_back(nodes[lineTokens[1]]);
            secondNode->addInverseSumOfResistance(resistor->value);

            if (secondNodeCoords[0] == 1)
                secondNode->isAbelToConnectCurrentSource = true;
            else if (secondNodeCoords[0] == 9)
                secondNode->isAbelToConnectVoltageSource = true;

            break;
        }

        case 'i':
        case 'I': {
            auto currentSource = std::make_shared<CurrentSource>(firstNodeCoords, value, lineTokens[0]);

            m_currentSources.push_back(currentSource);
            currentSources[lineTokens[0]] = CurrentSourcePtr(currentSource);

            if (!nodes.count(lineTokens[1])) {
                m_nodes.push_back(std::make_shared<Node>(firstNodeCoords, lineTokens[1]));
                nodes[lineTokens[1]] = NodePtr(m_nodes.back());
            }
            auto firstNode = nodes[lineTokens[1]];

            currentSource->connectedNode = nodes[lineTokens[1]];
            firstNode->connectCurrentSource(currentSources[lineTokens[0]]);

            // IR-drop preparations
            firstNode->isAbelToConnectCurrentSource = false;
            break;
        }

        case 'v':
        case 'V': {
            auto voltageSource = std::make_shared<VoltageSource>(firstNodeCoords, value, lineTokens[0]);

            m_voltageSources.push_back(voltageSource);
            voltageSources[lineTokens[0]] = VoltageSourcePtr(voltageSource);

            if (!nodes.count(lineTokens[1])) {
                m_nodes.push_back(std::make_shared<Node>(firstNodeCoords, lineTokens[1]));
                nodes[lineTokens[1]] = NodePtr(m_nodes.back());
            }

            auto firstNode = nodes[lineTokens[1]];

            voltageSource->connectedNode = nodes[lineTokens[1]];
            firstNode->connectVoltageSource(voltageSources[lineTokens[0]]);

            // IR-drop preparations
            firstNode->value += voltageSource->value;
            firstNode->isAbelToConnectVoltageSource = false;
            firstNode->isVoltageNode = true;

            m_voltageSourceValue = voltageSource->value;
            break;
        }

        default:
            break;
        }
    }
}

// =================================================================
// Change current sources

void PDNContainer::changeCurrentSourceValue(const Value& t_value)
{
    for (auto& currentSource : m_currentSources) {
        currentSource->value *= (1 + t_value);
    }
}


void PDNContainer::connectFakeCurrentSources(const Value& t_percentageConnect)
{
    std::vector<uint64_t> indexesToConnect {}; // Nodes indexes to connect current source

    for (size_t i {}; i < m_nodes.size(); ++i) {
        if (!m_nodes[i]->isFakedByCurrentSource && m_nodes[i]->isAbelToConnectCurrentSource) {
            indexesToConnect.push_back(i);
        }
    }

    if (indexesToConnect.size() > 0) {
        Value maxCurrentSourceValue {};
        Value minCurrentSourceValue {};

        for (auto& currentSource : m_currentSources) {
            maxCurrentSourceValue = std::max(maxCurrentSourceValue, currentSource->value);
            minCurrentSourceValue = std::min(minCurrentSourceValue, currentSource->value);
        }

        std::uniform_int_distribution<std::mt19937::result_type> distrConnect(0, indexesToConnect.size() - 1);
        std::set<uint64_t, std::greater<uint64_t>> randomIndexesToConnect {};

        uint64_t totalToBeConnected = ceil(indexesToConnect.size() * t_percentageConnect);

        for (size_t i {}; i < totalToBeConnected; ++i) {
            randomIndexesToConnect.insert(indexesToConnect.at(distrConnect(m_generator)));
        }

        std::uniform_real_distribution<> distrValue(minCurrentSourceValue, maxCurrentSourceValue);

        for (auto& index : randomIndexesToConnect) {
            std::stringstream currentSourceName;
            currentSourceName << "I" << m_currentSources.size() + 1;

            auto currentSource = std::make_shared<CurrentSource>(m_nodes.at(index)->getCoordinates(),
                distrValue(m_generator), currentSourceName.str());
            currentSource->connectedNode = m_nodes[index];

            m_nodes[index]->connectCurrentSource(currentSource);
            m_nodes[index]->isAbelToConnectCurrentSource = false;
            m_nodes[index]->isFakedByCurrentSource = true;
            m_currentSources.push_back(currentSource);
        }
    } else {
        changeCurrentSourceValue(t_percentageConnect);
    }
}

void PDNContainer::inverseCurrentSourcesPositions(const Value& t_percentage)
{
    std::vector<uint64_t> indexesToDisconnect {}; // Current source indexes to disconnect
    std::vector<uint64_t> indexesToConnect {}; // Nodes indexes to connect current source

    for (size_t i {}; i < m_nodes.size(); ++i) {
        if (!m_nodes[i]->isFakedByCurrentSource && m_nodes[i]->isAbelToConnectCurrentSource) {
            indexesToConnect.push_back(i);
        }
    }

    for (size_t i {}; i < m_currentSources.size(); ++i) {
        if (!m_currentSources[i]->connectedNode->isFakedByCurrentSource) {
            indexesToDisconnect.push_back(i);
        }
    }

    if (indexesToDisconnect.size() > 0 && indexesToConnect.size() > 0) {
        std::uniform_int_distribution<std::mt19937::result_type> distrDisconnect(0, indexesToDisconnect.size() - 1);
        std::uniform_int_distribution<std::mt19937::result_type> distrConnect(0, indexesToConnect.size() - 1);

        std::set<uint64_t, std::greater<uint64_t>> randomIndexesToDisconnect {};
        std::set<uint64_t, std::greater<uint64_t>> randomIndexesToConnect {};

        uint64_t totalToBeInverted = floor(indexesToDisconnect.size() * t_percentage);

        for (size_t i {}; i < totalToBeInverted; ++i) {
            randomIndexesToDisconnect.insert(indexesToDisconnect.at(distrDisconnect(m_generator)));
            randomIndexesToConnect.insert(indexesToConnect.at(distrConnect(m_generator)));
        }

        auto itDisconnect = randomIndexesToDisconnect.begin();
        auto itConnect = randomIndexesToConnect.begin();

        for (size_t i {}; i < totalToBeInverted; ++i) {
            // Save current source value
            uint64_t disconnectIndex = *itDisconnect;

            // Disconnect current source
            auto node = m_currentSources[disconnectIndex]->connectedNode;
            node->disconnectCurrentSource(m_currentSources.at(disconnectIndex));
            node->isAbelToConnectCurrentSource = true;
            node->isFakedByCurrentSource = true;

            // Connect current source
            uint64_t connectIndex = *itConnect;
            m_currentSources[disconnectIndex]->connectedNode = m_nodes[connectIndex];
            m_currentSources[disconnectIndex]->name += "_new";
            m_currentSources[disconnectIndex]->setNewCoords(m_nodes[connectIndex]->getCoordinates());

            m_nodes[connectIndex]->connectCurrentSource(m_currentSources[disconnectIndex]);
            m_nodes[connectIndex]->isAbelToConnectCurrentSource = false;
            m_nodes[connectIndex]->isFakedByCurrentSource = true;

            ++itDisconnect;
            ++itConnect;
        }
    } else {
        connectFakeCurrentSources(t_percentage);
    }
}

// =================================================================
// IR-drop methods

uint64_t PDNContainer::solveDC(const Value& t_precision, const uint64_t& t_maxIterations)
{
    uint64_t totalIterations {};
    auto nodesSize = m_nodes.size();

    for (auto& node : m_nodes) {
        node->reinitializationDC();
    }

    for (; totalIterations < t_maxIterations; ++totalIterations) {
        uint64_t totalNodesSolved {};

        for (auto& node : m_nodes) {
            totalNodesSolved += node->stepDC(t_precision);
        }

        if (totalNodesSolved == nodesSize)
            break;
    }

    return totalIterations;
}

void PDNContainer::solveDCAndSaveRealValues(const Value& t_precision, const uint64_t& t_maxIterations)
{
    for (auto& node : m_nodes) {
        node->initializationDC();
    }

    solveDC(t_precision, t_maxIterations);

    for (auto& node : m_nodes) {
        node->realValue = node->value;
    }
}

Value PDNContainer::compareFakeWithRealValues()
{
    std::vector<Value> percentageDifference {};
    Value meanDifference {};

    for (auto& node : m_nodes) {
        if (!node->isVoltageNode) {
            percentageDifference.push_back(
                std::fabs((node->realValue - node->value) / (m_voltageSourceValue - node->realValue)));
        }
    }

    for (auto& difference : percentageDifference) {
        meanDifference += difference;
    }

    meanDifference /= percentageDifference.size();

    return meanDifference;
}

std::array<Value, 3> PDNContainer::calculateIRDrop()
{
    std::array<Value, 3> irDropValues { 0, m_voltageSourceValue, 0 };

    for (auto& node : m_nodes) {
        if (!node->isVoltageNode) {
            Value tmp = m_voltageSourceValue - node->value;

            irDropValues[0] = std::max(irDropValues[0], tmp);
            irDropValues[1] = std::min(irDropValues[1], tmp);
            irDropValues[2] += tmp;
        }
    }

    irDropValues[2] /= m_nodes.size();

    return irDropValues;
}

// =================================================================
// Write/save methods

void PDNContainer::writeIRDropToFile(const std::string& t_fileName)
{
    std::ofstream file(t_fileName);

    if (file.is_open()) {
        file << "Nodes, Values\n"
             << std::flush;

        for (const auto& nodeInstance : m_nodes)
            file << nodeInstance->name << ", " << std::setprecision(16) << std::scientific << m_voltageSourceValue - nodeInstance->value << "\n";

        file.close();
    }
}

void PDNContainer::writeNetlistToFile(const std::string& t_fileName)
{
    std::ofstream file(t_fileName);

    if (file.is_open()) {
        for (const auto& resistorInstance : m_resistors)
            file << resistorInstance->name << " " << resistorInstance->toString() << "\n"
                 << std::flush;

        for (const auto& voltageSourceInstance : m_voltageSources)
            file << voltageSourceInstance->name << " " << voltageSourceInstance->toString() << "\n"
                 << std::flush;

        for (const auto& currentSourceInstance : m_currentSources)
            file << currentSourceInstance->name << " " << currentSourceInstance->toString() << "\n"
                 << std::flush;

        file.close();
    }
}