#pragma once

#include "simulation/circuit.h"
#include "simulation/commands/command.h"

class DisconnectComponentCommand : public Command
{
public:
    void execute(Circuit& circuit) override;
    void undo(Circuit& circuit) override;
    void redo(Circuit& circuit) override;

    // Members
};

inline void DisconnectComponentCommand::execute(Circuit& circuit)
{
}

inline void DisconnectComponentCommand::undo(Circuit& circuit)
{
}

inline void DisconnectComponentCommand::redo(Circuit& circuit)
{
}
