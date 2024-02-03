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


class Node;
class NandGate;
typedef std::shared_ptr<Node>     NodePtr;
typedef std::shared_ptr<NandGate> GatePtr;

// TODO: These gate and circuit implementations were created by ChatGPT to get _something_ running underneath the UI.
//       They are not intended to be a final solution. They are a starting point for a more robust & fast implementation.
//       Presumably they don't handle cycles, and they don't handle multiple inputs/outputs.
//       We also need an abstraction of a Bus with a specialisation for a single bit (Bus<1> == Wire).
class Node
{
public:
    bool value;
    bool stateChanged;

    Node(bool initialValue = false)
        : value(initialValue), stateChanged(false)
    {
    }

    inline virtual void setValue(bool val)
    {
        if (value != val)
        {
            value = val;
            stateChanged = true;
        }
    }

    inline virtual void simulate()
    {
        // Base class does nothing
    }

    inline bool getStateChanged() const
    {
        return stateChanged;
    }

    inline void resetStateChanged()
    {
        stateChanged = false;
    }
};

class ConstantInputNode : public Node
{
public:
    ConstantInputNode(bool initialValue)
        : Node(initialValue)
    {
    }

    inline void setValue(bool val) override
    {
        // Do nothing to prevent value change
    }
};

class OutputNode : public Node
{
    bool logOutput;

public:
    OutputNode() : Node(), logOutput(true) {}

    inline void simulate() override
    {
        if (logOutput)
        {
            // spdlog::info(fmt::format("Output Value: {}", value).c_str());
        }
    }

    inline void setLogOutput(bool shouldLog)
    {
        logOutput = shouldLog;
    }
};

class ClockNode : public Node
{
public:
    ClockNode()
        : Node()
    {
    }

    inline void simulate() override
    {
        setValue(!value);
    }
};

class NandGate
{
    NodePtr input1, input2, output;

public:
    NandGate(NodePtr in1, NodePtr in2, NodePtr out)
        : input1(in1), input2(in2), output(out)
    {
    }

    inline void operate()
    {
        output->setValue(!(input1->value && input2->value));
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
    std::vector<NodePtr> nodes;
    std::vector<GatePtr> gates;

    std::vector<NodePtr> inputNodes;
    std::vector<NodePtr> outputNodes;
    std::vector<NodePtr> clockNodes;

    std::queue<NodePtr> bfsQueue;

public:
    LogicSim()
    {
        spdlog::info("LogicSim::LogicSim()");

        // Test circuit
        NodePtr input       = addConstantInputNode(true);
        NodePtr clock       = addClockNode();
        NodePtr finalOutput = addOutputNode();

        // Create NAND gate
        auto nandGate = addGate();
        nandGate->connectInput1(input);
        nandGate->connectInput2(clock);

        // Create a chain
        for (int i = 0; i < std::pow(2, 8); ++i)
        {
            auto newGate = addGate();
            newGate->connectInput1(nandGate->getOutputNode());
            newGate->connectInput2(nandGate->getOutputNode());
            nandGate = newGate;
        }

        // Hook up final output
        nandGate->connectOutput(finalOutput);

        // Perform an initial simulation to make sure the initial state is correct
        for (int i = 0; i < 4; ++i)
        {
            step(false);
        }
    }

    ~LogicSim()
    {
    }

    NodePtr addNode()
    {
        // std::lock_guard<std::shared_mutex> lock(readWriteMutex); // WRITE LOCK
        auto newNode = std::make_shared<Node>();
        nodes.push_back(newNode);
        return newNode;
    }

    NodePtr addConstantInputNode(bool initialValue)
    {
        // std::lock_guard<std::shared_mutex> lock(readWriteMutex); // WRITE LOCK
        auto newNode = std::make_shared<ConstantInputNode>(initialValue);
        nodes.push_back(newNode);
        inputNodes.push_back(newNode);
        return newNode;
    }

    NodePtr addOutputNode()
    {
        // std::lock_guard<std::shared_mutex> lock(readWriteMutex); // WRITE LOCK
        auto newNode = std::make_shared<OutputNode>();
        nodes.push_back(newNode);
        outputNodes.push_back(newNode);
        return newNode;
    }

    NodePtr addClockNode()
    {
        // std::lock_guard<std::shared_mutex> lock(readWriteMutex); // WRITE LOCK
        auto newNode = std::make_shared<ClockNode>();
        nodes.push_back(newNode);
        clockNodes.push_back(newNode);
        return newNode;
    }

    GatePtr addGate()
    {
        // std::lock_guard<std::shared_mutex> lock(readWriteMutex); // WRITE LOCK
        auto newGate = std::make_shared<NandGate>(addNode(), addNode(), addNode());
        gates.push_back(newGate);
        return newGate;
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
            if (auto outputNode = std::dynamic_pointer_cast<OutputNode>(node))
            {
                outputNode->setLogOutput(logOutput);
            }
        }

        // Perform simulation with BFS
        for (auto& node : nodes)
        {
            node->simulate(); // Ensure every node gets a chance to simulate
            if (node->getStateChanged())
            {
                bfsQueue.push(node);
                node->resetStateChanged();
            }
        }

        while (!bfsQueue.empty())
        {
            auto current = bfsQueue.front();
            bfsQueue.pop();

            for (auto& gate : gates)
            {
                gate->operate();
                if (gate->getOutputNode()->getStateChanged())
                {
                    bfsQueue.push(gate->getOutputNode());
                    gate->getOutputNode()->resetStateChanged();
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
    std::mutex mutex; // This is needed for Step() to be thread safe from the UI
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
