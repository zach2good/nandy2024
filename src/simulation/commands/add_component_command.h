#pragma once

#include "simulation/circuit.h"
#include "simulation/commands/command.h"

class AddComponentCommand : public Command
{
public:
    void execute(Circuit& circuit) override;
    void undo(Circuit& circuit) override;
    void redo(Circuit& circuit) override;

    // Members
};

inline void AddComponentCommand::execute(Circuit& circuit)
{
}

inline void AddComponentCommand::undo(Circuit& circuit)
{
}

inline void AddComponentCommand::redo(Circuit& circuit)
{
}
