#pragma once

#include "gates.h"

#include "common.h"

#include <atomic>
#include <vector>
#include <queue>
#include <unordered_set>
#include <cstdint>
#include <algorithm>
#include <string>
#include <mutex>
#include <shared_mutex>

class Component;
class Node;
class NandGate;

typedef std::shared_ptr<Component> ComponentPtr;
typedef std::shared_ptr<Node>      NodePtr;
typedef std::shared_ptr<NandGate>  GatePtr;

typedef std::vector<std::shared_ptr<Component>> ComponentPtrs;

enum ComponentType
{
    NONE,
    NODE,
    NAND,
    NOTE,
};

static std::string componentTypeToString(ComponentType type)
{
    switch (type)
    {
        case NONE: return "NONE";
        case NODE: return "NODE";
        case NAND: return "NAND";
        case NOTE: return "NOTE";
        default: return "UNKNOWN";
    }
}

static float kNandGateSize   = 100.0f;
static float kNodeSize       = 10.0f;
static u64   nextComponentId = 1;

class Component
{
public:
    Component(ComponentType type)
        : type(type)
        , id(nextComponentId++)
    {
    }
    virtual ~Component() = default;

    virtual ComponentPtrs simulate() = 0;

    ComponentType type  = NONE;
    u64           id    = 0;
    bool          dirty = true;

    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};

// TODO: These gate and circuit implementations were created by ChatGPT to get _something_ running underneath the UI.
//       They are not intended to be a final solution. They are a starting point for a more robust & fast implementation.
//       Presumably they don't handle cycles, and they don't handle multiple inputs/outputs.
//       We also need an abstraction of a Bus with a specialisation for a single bit (Bus<1> == Wire).
class Node : public Component
{
private:
    bool value;

public:
    GatePtr       parent;
    ComponentPtrs driving;

    Node(bool initialValue = false)
    : Component(NODE)
    , value(initialValue)
    {
    }

    bool getValue() const
    {
        return value;
    }

    void setValue(bool newValue)
    {
        value = newValue;
        dirty = true; // TODO: Compare this to the original value
    }

    inline ComponentPtrs simulate() override
    {
        dirty = false;
        for (auto const& component : driving)
        {
            if (component->type == NODE)
            {
                auto node = std::static_pointer_cast<Node>(component);
                if (node->value != value)
                {
                    node->value = value;
                    node->dirty = true;
                }
            }
            else // TODO: Check NAND value?
            {
                component->dirty = true;
            }
        }
        return driving;
    }
};

class OutputNode final : public Node
{
private:
    bool logOutput;

public:
    OutputNode() : Node(), logOutput(true) {}

    inline ComponentPtrs simulate() override
    {
        if (logOutput)
        {
            spdlog::info(fmt::format("Output Value ({}): {}", id, getValue()).c_str());
        }
        return Node::simulate();
    }

    inline void setLogOutput(bool shouldLog)
    {
        logOutput = shouldLog;
    }
};

class ClockNode final : public Node
{
public:
    ClockNode()
    : Node()
    {
    }

    inline ComponentPtrs simulate() override
    {
        setValue(!getValue());
        return Node::simulate();
    }
};

class NandGate final : public Component
{
private:
    NodePtr input1;
    NodePtr input2;
    NodePtr output;

public:
    NandGate(NodePtr in1, NodePtr in2, NodePtr out)
    : Component(NAND)
    , input1(in1)
    , input2(in2)
    , output(out)
    {
    }

    inline ComponentPtrs simulate() override
    {
        dirty = false;
        input1->dirty = false;
        input2->dirty = false;
        output->setValue(!(input1->getValue() && input2->getValue()));
        output->dirty = true;
        return { output };
    }

    // Access methods
    inline NodePtr getInputNode1() const { return input1; }
    inline NodePtr getInputNode2() const { return input2; }
    inline NodePtr getOutputNode() const { return output; }

    // Connection methods
    inline void connectInput1(NodePtr node) { input1 = node; }
    inline void connectInput2(NodePtr node) { input2 = node; }
    inline void connectOutput(NodePtr node) { output = node; }
};

class LogicSim
{
public:
    std::vector<NodePtr>     nodes;
    std::vector<GatePtr>     gates;
    std::queue<ComponentPtr> bfsQueue;

public:
    LogicSim()
    {
        spdlog::info("LogicSim::LogicSim()");
    }

    ~LogicSim()
    {
    }

    void reset()
    {
        std::lock_guard lock(mutex);

        nodes.clear();
        gates.clear();
        while (!bfsQueue.empty())
        {
            bfsQueue.pop();
        }
    }

    NodePtr addNode(float x, float y)
    {
        std::lock_guard lock(mutex);

        // std::lock_guard<std::shared_mutex> lock(readWriteMutex); // WRITE LOCK
        auto newNode = std::make_shared<Node>();
        newNode->x = x - (kNodeSize / 2.0f); // TODO: Don't hardcode this to make the node centers on the gate leg
        newNode->y = y - (kNodeSize / 2.0f); // TODO: Don't hardcode this to make the node centers on the gate leg
        newNode->w = kNodeSize;
        newNode->h = kNodeSize;
        nodes.push_back(newNode);
        return newNode;
    }

    NodePtr addOutputNode(float x, float y)
    {
        std::lock_guard lock(mutex);

        // std::lock_guard<std::shared_mutex> lock(readWriteMutex); // WRITE LOCK
        auto newNode = std::make_shared<OutputNode>();
        newNode->x = x - (kNodeSize / 2.0f); // TODO: Don't hardcode this to make the node centers on the gate leg
        newNode->y = y - (kNodeSize / 2.0f); // TODO: Don't hardcode this to make the node centers on the gate leg
        newNode->w = kNodeSize;
        newNode->h = kNodeSize;
        nodes.push_back(newNode);
        return newNode;
    }

    NodePtr addClockNode(float x, float y)
    {
        std::lock_guard lock(mutex);

        // std::lock_guard<std::shared_mutex> lock(readWriteMutex); // WRITE LOCK
        auto newNode = std::make_shared<ClockNode>();
        newNode->x = x - (kNodeSize / 2.0f); // TODO: Don't hardcode this to make the node centers on the gate leg
        newNode->y = y - (kNodeSize / 2.0f); // TODO: Don't hardcode this to make the node centers on the gate leg
        newNode->w = kNodeSize;
        newNode->h = kNodeSize;
        nodes.push_back(newNode);
        return newNode;
    }

    GatePtr addGate(float x, float y)
    {
        std::lock_guard lock(mutex);
        // std::lock_guard<std::shared_mutex> lock(readWriteMutex); // WRITE LOCK

        // Create a new gate using the ratios above
        auto w = kNandGateSize;
        auto h = kNandGateSize;
        auto newGate = std::make_shared<NandGate>(addNode(x + w * 0.0f, y + h * 0.3f), addNode(x + w * 0.0f, y + h * 0.7f), addNode(x + w * 1.0f, y + h * 0.5f));
        newGate->x = x;
        newGate->y = y;
        newGate->w = w;
        newGate->h = h;

        newGate->getInputNode1()->parent = newGate;
        newGate->getInputNode1()->driving.push_back(newGate);

        newGate->getInputNode2()->parent = newGate;
        newGate->getInputNode2()->driving.push_back(newGate); // TODO: Is this creating more work?

        newGate->getOutputNode()->parent = newGate;
        gates.push_back(newGate);
        return newGate;
    }

    void connect(NodePtr in, NodePtr out)
    {
        std::lock_guard lock(mutex);
        in->driving.push_back(out);
    }

    void step(bool logOutput = true)
    {
        // Protect from the UI thread
        std::lock_guard lock(mutex);

        m_OpsPerStep = 0;
        auto startTime = std::chrono::high_resolution_clock::now();

        // Set logging for OutputNodes
        for (auto& node : nodes)
        {
            if (auto outputNode = std::dynamic_pointer_cast<OutputNode>(node); outputNode != nullptr)
            {
                outputNode->setLogOutput(logOutput);
            }

            if (auto clockNode = std::dynamic_pointer_cast<ClockNode>(node); clockNode != nullptr)
            {
                // TODO: This is a hack to get the clock to work. It should be done differently.
                clockNode->dirty = true;
            }

            if (node->dirty)
            {
                bfsQueue.push(node);
            }
        }

        while (!bfsQueue.empty())
        {
            auto current = bfsQueue.front();
            bfsQueue.pop();

            for (auto const& next : current->simulate())
            {
                if (next->dirty)
                {
                    bfsQueue.push(next);
                }
            }

            ++m_OpsPerStep;
            ++m_OpsTotalCount;
        }
        ++m_StepCount;

        auto endTime   = std::chrono::high_resolution_clock::now();
        auto timeTaken = endTime - startTime;
        m_StepTime     = std::chrono::duration_cast<std::chrono::microseconds>(timeTaken).count();
    }

    void run()
    {
        // std::lock_guard<std::recursive_mutex> lock(readWriteMutex); // WRITE LOCK
        m_IsRunning = true;
    }

    void stop()
    {
        // std::lock_guard<std::recursive_mutex> lock(readWriteMutex); // WRITE LOCK
        m_IsRunning = false;
    }

    bool running()
    {
        // std::shared_lock lock(readWriteMutex); // READ LOCK
        return m_IsRunning;
    }

    u64 stepCount()
    {
        // std::shared_lock lock(readWriteMutex); // READ LOCK
        return m_StepCount;
    }

    float stepTime()
    {
        // std::shared_lock lock(readWriteMutex); // READ LOCK
        return m_StepTime;
    }

    u64 opsTotalCount()
    {
        // std::shared_lock lock(readWriteMutex); // READ LOCK
        return m_OpsTotalCount;
    }

    u64 opsPerStep()
    {
        // std::shared_lock lock(readWriteMutex); // READ LOCK
        return m_OpsPerStep;
    }

private:
    // TODO: With 1025 gates in a chain the simulation ran at 350Hz without this lock. Check to see if this slows it down substantially.
    // TODO: Need shared_recursive_mutex to allow for recursive locking
    std::recursive_mutex mutex; // This is needed for Step() to be thread safe from the UI
    // TODO: This locks up

    // TODO: These don't have to be atomic if we lock properly
    std::atomic<bool>  m_IsRunning;
    std::atomic<u64>   m_StepCount;
    std::atomic<float> m_StepTime;
    std::atomic<u64>   m_OpsTotalCount;
    std::atomic<u64>   m_OpsPerStep;
};

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
