#pragma once

#include "simulation/circuit.h"
#include "simulation/commands/command.h"

class RemoveComponentsCommand : public Command
{
public:
    void execute(Circuit& circuit) override;
    void undo(Circuit& circuit) override;
    void redo(Circuit& circuit) override;

    // Members
};

inline void RemoveComponentsCommand::execute(Circuit& circuit)
{
}

inline void RemoveComponentsCommand::undo(Circuit& circuit)
{
}

inline void RemoveComponentsCommand::redo(Circuit& circuit)
{
}
