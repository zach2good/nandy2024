#pragma once

#include "gates.h"

#include "common.h"

#include <array>
#include <atomic>
#include <vector>
#include <queue>
#include <unordered_set>
#include <cstdint>
#include <algorithm>
#include <string>
#include <variant>
#include <thread>
#include <chrono>
#include <mutex>
#include <list>
#include <shared_mutex>
#include <fstream>
#include <iostream>
#include <sstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace nlohmann::literals;

using u64 = std::uint64_t;

enum ComponentType
{
    NONE,
    NODE,
    NAND,
    NOTE,
};

static const std::string kComponentTypeStrings[] = {
    "NONE",
    "NODE",
    "NAND",
    "NOTE",
};

struct ComponentId { u64 value; };
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ComponentId, value)
struct NodeId { u64 value; };
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NodeId, value)
struct GateId { u64 value; };
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GateId, value)

static float kNandGateSize = 100.0f;
static float kNodeSize     = 10.0f;

// Glue to serialize NodeId and GateId to json
typedef std::variant<NodeId, GateId> VariantType;
namespace nlohmann
{
template <>
struct adl_serializer<VariantType>
{
    static void to_json(json& j, const VariantType& value)
    {
        if (auto nodeId = std::get_if<NodeId>(&value))
        {
            j = json{ {"type", 0}, {"value", nodeId->value} };
        }
        else if (auto gateId = std::get_if<GateId>(&value))
        {
            j = json{ {"type", 1}, {"value", gateId->value} };
        }
    }

    static void from_json(const json& j, VariantType& value)
    {
        if (j.at("type").get<int>() == 0)
        {
            value = NodeId{ j.at("value").get<u64>() };
        }
        else if (j.at("type").get<int>() == 1)
        {
            value = GateId{ j.at("value").get<u64>() };
        }
    }
};
}

struct Component
{
    ComponentType type   = NONE;
    ComponentId   id     = {};
    VariantType   implId = {};
    bool          dirty  = true;
    float         x      = 0.0f;
    float         y      = 0.0f;
    float         w      = 0.0f;
    float         h      = 0.0f;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Component, type, id, implId, x, y, w, h)

struct Node
{
    NodeId      id          = {};
    ComponentId componentId = {};
    bool        value       = false;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Node, id, componentId)

struct NandGate
{
    GateId      id          = {};
    ComponentId componentId = {};
    NodeId      input0Id    = {};
    NodeId      input1Id    = {};
    NodeId      outputId    = {};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NandGate, id, componentId, input0Id, input1Id, outputId)

class LogicSim
{
public:
    // TODO: Consider Structure-of-Arrays instead of Array-of-Structures
    std::vector<Component>           components;
    std::vector<Node>                nodes;
    std::vector<NandGate>            gates;
    std::vector<std::vector<NodeId>> drivingNodes;
    std::vector<NodeId>              clockNodes;

    // TODO: Use this
    std::vector<NodeId> dirtyNodes;

    // TODO: replace this with something more efficient
    std::queue<Component> bfsQueue;

    void sanityTests()
    {
        // Basic Node Chain
        {
            auto n0 = addNode(0, 0);
            auto n1 = addNode(0, 0);
            auto n2 = addNode(0, 0);

            connect(n0, n1);
            connect(n1, n2);

            setNodeValue(n0, true);

            step();

            assert(getNodeValue(n0) == true);
            assert(getNodeValue(n1) == true);
            assert(getNodeValue(n2) == true);

            reset();
        }

        // Basic NAND Gate
        {
            auto n0 = addNode(0, 0);
            auto n1 = addNode(0, 0);
            auto n2 = addNode(0, 0);

            auto g0 = addGate(0, 0);

            connect(n0, getGateInput0Id(g0));
            connect(n1, getGateInput1Id(g0));
            connect(getGateOutputId(g0), n2);

            setNodeValue(n0, true);
            setNodeValue(n1, true);

            step();

            assert(getNodeValue(n0) == true);
            assert(getNodeValue(n1) == true);
            assert(getNodeValue(n2) == false);

            reset();
        }

        // Basic AND Gate (NAND + NAND as NOT)
        {
            auto n0 = addNode(0, 0);
            auto n1 = addNode(0, 0);
            auto n2 = addNode(0, 0);
            auto n3 = addNode(0, 0);

            auto g0 = addGate(0, 0);
            auto g1 = addGate(0, 0);

            connect(n0, getGateInput0Id(g0));
            connect(n1, getGateInput1Id(g0));
            connect(getGateOutputId(g0), n2);

            connect(n2, getGateInput0Id(g1));
            connect(n2, getGateInput1Id(g1));
            connect(getGateOutputId(g1), n3);

            setNodeValue(n0, true);
            setNodeValue(n1, true);

            step();

            assert(getNodeValue(n0) == true);
            assert(getNodeValue(n1) == true);
            assert(getNodeValue(n2) == false);
            assert(getNodeValue(n3) == true);

            reset();
        }

        // Clock Test
        {
            auto n0 = addClockNode(0, 0);
            auto n1 = addNode(0, 0);

            auto g0 = addGate(0, 0);

            connect(n0, getGateInput0Id(g0));
            connect(n1, getGateInput1Id(g0));

            setNodeValue(n0, false);
            setNodeValue(n1, true);

            step();

            assert(getNodeValue(n0) == true);
            assert(getNodeValue(n1) == true);
            assert(getNodeValue(getGateOutputId(g0)) == false);

            step();

            assert(getNodeValue(n0) == false);
            assert(getNodeValue(n1) == true);
            assert(getNodeValue(getGateOutputId(g0)) == true);

            reset();
        }
    }

public:
    LogicSim()
    {
        spdlog::info("LogicSim()");
        components.reserve(1024);
        nodes.reserve(1024);
        gates.reserve(1024);
        drivingNodes.reserve(1024);
        clockNodes.reserve(1024);
        dirtyNodes.reserve(1024);

        sanityTests();

        loadFromFile("circuit.json");
    }

    ~LogicSim()
    {
    }

    LogicSim(const LogicSim& other)
    {
        components = other.components;
        nodes      = other.nodes;
        gates      = other.gates;
        drivingNodes = other.drivingNodes;
        clockNodes = other.clockNodes;
        dirtyNodes = other.dirtyNodes;
    }

    LogicSim(LogicSim&& other)
    {
        components = std::move(other.components);
        nodes      = std::move(other.nodes);
        gates      = std::move(other.gates);
        drivingNodes = std::move(other.drivingNodes);
        clockNodes = std::move(other.clockNodes);
        dirtyNodes = std::move(other.dirtyNodes);
    }

    LogicSim& operator=(const LogicSim& other)
    {
        if (this != &other)
        {
            components = other.components;
            nodes      = other.nodes;
            gates      = other.gates;
            drivingNodes = other.drivingNodes;
            clockNodes = other.clockNodes;
            dirtyNodes = other.dirtyNodes;
        }
        return *this;
    }

    void reset()
    {
        components.clear();
        nodes.clear();
        gates.clear();
        drivingNodes.clear();
        clockNodes.clear();
        dirtyNodes.clear();

        while (!bfsQueue.empty())
        {
            bfsQueue.pop();
        }
    }

    NodeId addNode(float x, float y, bool dirty = true)
    {
        u64 componentId = components.size();
        u64 nodeId      = nodes.size();

        Component newComponent;
        newComponent.id     = { componentId };
        newComponent.implId = NodeId{ nodeId };
        newComponent.type   = NODE;
        newComponent.dirty  = dirty;
        newComponent.x      = x - (kNodeSize / 2.0f);
        newComponent.y      = y - (kNodeSize / 2.0f);
        newComponent.w      = kNodeSize;
        newComponent.h      = kNodeSize;
        components.push_back(newComponent);

        Node newNode;
        newNode.id          = { nodeId };
        newNode.componentId = { componentId };
        newNode.value       = false;
        nodes.push_back(newNode);

        // This is aligned with this node's nodeId
        drivingNodes.push_back({});

        return nodes.back().id;
    }

    NodeId addClockNode(float x, float y, bool dirty = true)
    {
        auto id = addNode(x, y, dirty);
        clockNodes.push_back(id);
        return id;
    }

    GateId addGate(float x, float y)
    {
        u64 componentId = components.size();
        u64 gateId      = gates.size();

        Component newComponent;
        newComponent.id     = { componentId };
        newComponent.implId = GateId{ gateId };
        newComponent.type   = NAND;
        newComponent.dirty  = true;
        newComponent.x      = x;
        newComponent.y      = y;
        newComponent.w      = kNandGateSize;
        newComponent.h      = kNandGateSize;
        components.push_back(newComponent);

        NandGate newGate;
        newGate.id          = { gateId };
        newGate.componentId = { componentId };
        newGate.input0Id    = addNode(x, y + kNandGateSize * 0.3f, false);
        newGate.input1Id    = addNode(x, y + kNandGateSize * 0.7f, false);
        newGate.outputId    = addNode(x + kNandGateSize, y + kNandGateSize * 0.5f, false);
        gates.push_back(newGate);

        return gates.back().id;
    }

    void connect(NodeId inIdx, NodeId outIdx)
    {
        // TODO: Minimise lookups
        auto& inputNode      = nodes[inIdx.value];
        auto& inputComponent = components[inputNode.componentId.value];
        auto& driving        = drivingNodes[inIdx.value];

        driving.push_back(outIdx);
        inputComponent.dirty = true;
    }

    inline void setNodeValue(Node& id, bool newValue)
    {
        // TODO: Minimise lookups
        Node& node           = nodes[id.value];
        Component& component = components[node.componentId.value];

        if (node.value != newValue)
        {
            node.value      = newValue;
            component.dirty = true;
        }
    }

    inline void setNodeValue(NodeId id, bool newValue)
    {
        // TODO: Minimise lookups
        Node& node           = nodes[id.value];
        Component& component = components[node.componentId.value];

        if (node.value != newValue)
        {
            node.value      = newValue;
            component.dirty = true;
        }
    }

    inline bool getNodeValue(NodeId id)
    {
        return nodes[id.value].value;
    }

    inline NodeId getGateInput0Id(GateId id)
    {
        auto& gate = gates[id.value];
        return gate.input0Id;
    }

    inline NodeId getGateInput1Id(GateId id)
    {
        auto& gate = gates[id.value];
        return gate.input1Id;
    }

    inline NodeId getGateOutputId(GateId id)
    {
        auto& gate = gates[id.value];
        return gate.outputId;
    }

    inline Node& getNode(NodeId id)
    {
        return nodes[id.value];
    }

    inline Node& getNode(ComponentId id)
    {
        return nodes[id.value];
    }

    inline Node& getNode(Component& component)
    {
        return nodes[std::get<NodeId>(component.implId).value];
    }

    inline NandGate& getGate(GateId id)
    {
        return gates[id.value];
    }

    inline NandGate& getGate(ComponentId id)
    {
        return gates[id.value];
    }

    inline NandGate& getGate(Component& component)
    {
        return gates[std::get<GateId>(component.implId).value];
    }

private:
    void simulateNode(Component& component, Node& node, std::queue<Component>& bfsQueue)
    {
        component.dirty = false;

        // TODO: If the node state changes, mark connected components as dirty
        for (auto& connectedComponentId : drivingNodes[node.id.value])
        {
            // TODO: Minimise lookups
            Component& connectedComponent = components[connectedComponentId.value];
            Node&      connectedNode      = nodes[connectedComponentId.value];

            if (node.value != connectedNode.value)
            {
                connectedNode.value      = node.value;
                connectedComponent.dirty = true;

                bfsQueue.push(connectedComponent);
            }
        }
    }

    void simulateNandGate(Component& component, NandGate& gate, std::queue<Component>& bfsQueue)
    {
        component.dirty = false;

        // TODO: Minimise lookups
        Node& input0 = nodes[gate.input0Id.value];
        Node& input1 = nodes[gate.input1Id.value];

        bool newOutputValue = !(input0.value && input1.value);

        Node& outputNode = nodes[gate.outputId.value];

        if (outputNode.value != newOutputValue)
        {
            Component& outputComponent = components[outputNode.componentId.value];

            outputNode.value      = newOutputValue;
            outputComponent.dirty = true;

            bfsQueue.push(outputComponent);
        }
    }

public:
    void step()
    {
        m_OpsPerStep = 0;
        auto startTime = std::chrono::high_resolution_clock::now();

        for (auto& clockNode : clockNodes)
        {
            setNodeValue(clockNode, !getNodeValue(clockNode));
        }

        for (auto& component : components)
        {
            if (component.dirty)
            {
                bfsQueue.push(component);
            }
        }

        while (!bfsQueue.empty())
        {
            auto component = bfsQueue.front();
            bfsQueue.pop();

            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, NodeId>)
                {
                    Node& node = nodes[arg.value];
                    simulateNode(component, node, bfsQueue);
                }
                else if constexpr (std::is_same_v<T, GateId>)
                {
                    NandGate& gate = gates[arg.value];
                    simulateNandGate(component, gate, bfsQueue);
                }
            }, component.implId);

            /*
            if (component.type == NODE)
            {
                Node& node = nodes[std::get<NodeId>(component.implId).value];
                simulateNode(component, node, bfsQueue);
            }
            else if (component.type == NAND)
            {
                NandGate& gate = gates[std::get<GateId>(component.implId).value];
                simulateNandGate(component, gate, bfsQueue);
            }
            */

            ++m_OpsPerStep;
            ++m_OpsTotalCount;
        }
        ++m_StepCount;

        auto endTime   = std::chrono::high_resolution_clock::now();
        auto timeTaken = endTime - startTime;
        m_StepTime     = std::chrono::duration_cast<std::chrono::nanoseconds>(timeTaken).count();
    }

    void run()
    {
        m_IsRunning = true;
    }

    void stop()
    {
        m_IsRunning = false;
    }

    bool running()
    {
        return m_IsRunning;
    }

    u64 stepCount()
    {
        return m_StepCount;
    }

    float stepTime()
    {
        return m_StepTime;
    }

    u64 opsTotalCount()
    {
        return m_OpsTotalCount;
    }

    u64 opsPerStep()
    {
        return m_OpsPerStep;
    }

    std::string toJson()
    {
        json j;
        j["components"] = components;
        j["nodes"]      = nodes;
        j["gates"]      = gates;
        j["drivingNodes"] = drivingNodes;
        j["clockNodes"] = clockNodes;
        return j.dump(4);
    }

    static LogicSim fromJson(const std::string& jsonStr)
    {
        json j = json::parse(jsonStr);
        LogicSim sim;
        j.at("components").get_to(sim.components);
        j.at("nodes").get_to(sim.nodes);
        j.at("gates").get_to(sim.gates);
        j.at("drivingNodes").get_to(sim.drivingNodes);
        j.at("clockNodes").get_to(sim.clockNodes);
        return std::move(sim);
    }

    void fromJsonInPlace(const std::string& jsonStr)
    {
        if (jsonStr.empty())
        {
            return;
        }

        reset();

        json j = json::parse(jsonStr);
        j.at("components").get_to(components);
        j.at("nodes").get_to(nodes);
        j.at("gates").get_to(gates);
        j.at("drivingNodes").get_to(drivingNodes);
        j.at("clockNodes").get_to(clockNodes);
    }

    void saveToFile(const std::string& filename)
    {
        std::ofstream file(filename);
        file << toJson();
    }

    void loadFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        fromJsonInPlace(buffer.str());
    }

private:
    // TODO: These don't have to be atomic if we lock properly
    std::atomic<bool>  m_IsRunning;
    std::atomic<u64>   m_StepCount;
    std::atomic<float> m_StepTime;
    std::atomic<u64>   m_OpsTotalCount;
    std::atomic<u64>   m_OpsPerStep;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogicSim, components, nodes, gates, drivingNodes, clockNodes, dirtyNodes)

// TODO: Pack node values into these?
/*
std::vector<u64> gateValues;
u64 getGateValue(u64 gateIndex)
{
    u64 byteIndex = gateIndex / 64;
    u64 bitIndex = gateIndex % 64;

    if (byteIndex >= gateValues.size())
    {
        throw std::runtime_error("getGateValue index out of range");
    }

    return (gateValues[byteIndex] >> bitIndex) & 1;
}

void setGateValue(u64 gateIndex, bool value)
{
    u64 byteIndex = gateIndex / 64;
    u64 bitIndex = gateIndex % 64;

    if (byteIndex >= gateValues.size())
    {
        gateValues.resize(byteIndex + 1);
    }

    if (value)
    {
        gateValues[byteIndex] |= (u64(1) << bitIndex);
    }
    else
    {
        gateValues[byteIndex] &= ~(u64(1) << bitIndex);
    }
}
*/
