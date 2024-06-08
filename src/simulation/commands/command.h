#pragma once

#include "simulation/circuit.h"
#include "simulation/commands/command.h"

class Command
{
public:
    virtual ~Command() = default;

    virtual void execute(Circuit& circuit) = 0;
    virtual void undo(Circuit& circuit)    = 0;
    virtual void redo(Circuit& circuit)    = 0;
};
