#pragma once

#include "components/component.h"
#include "components/nand_gate.h"

#include "types.h"

#include <memory>
#include <vector>

class Circuit
{
public:
    Circuit();
    ~Circuit();

    void step();

    void addNAND(Position position);

    // private:
    // components
    std::vector<Component*> components;
    // nodes
    // wires
};

inline Circuit::Circuit()
{
}

inline Circuit::~Circuit()
{
    // TODO: RAII
    for (auto component : components)
    {
        delete component;
    }
}

inline void Circuit::step()
{
}

inline void Circuit::addNAND(Position position)
{
    components.push_back(new NandGate(position));
}
