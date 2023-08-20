class Node;
class Resistor;
class CurrentSource;
class VoltageSource;

using L = uint8_t;
using X = uint32_t;
using Y = uint32_t;
using Value = double;
using Name = std::string;
using NodeCoords = std::array<uint32_t, 3>;

using NodePtr = std::shared_ptr<Node>;
using ResistorPtr = std::shared_ptr<Resistor>;
using CurrentSourcePtr = std::shared_ptr<CurrentSource>;
using VoltageSourcePtr = std::shared_ptr<VoltageSource>;
using NodePtrVec = std::vector<NodePtr>;
using ResistorPtrVec = std::vector<ResistorPtr>;
using CurrentSourcePtrVec = std::vector<CurrentSourcePtr>;
using VoltageSourcePtrVec = std::vector<VoltageSourcePtr>;