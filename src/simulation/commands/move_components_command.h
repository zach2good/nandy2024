#pragma once

#include "simulation/circuit.h"
#include "simulation/commands/command.h"

class MoveComponentsCommand : public Command
{
public:
    void execute(Circuit& circuit) override;
    void undo(Circuit& circuit) override;
    void redo(Circuit& circuit) override;

    // Members
};

inline void MoveComponentsCommand::execute(Circuit& circuit)
{
}

inline void MoveComponentsCommand::undo(Circuit& circuit)
{
}

inline void MoveComponentsCommand::redo(Circuit& circuit)
{
}
