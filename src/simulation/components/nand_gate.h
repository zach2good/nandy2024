#pragma once

#include "simulation/components/component.h"

struct NandGate : public Component
{
    NandGate(Position position)
    : position(position)
    {
    }

    virtual ~NandGate() = default;

    Position position;
};
