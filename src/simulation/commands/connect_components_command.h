#pragma once

#include "simulation/circuit.h"
#include "simulation/commands/command.h"

class ConnectComponentsCommand : public Command
{
public:
    void execute(Circuit& circuit) override;
    void undo(Circuit& circuit) override;
    void redo(Circuit& circuit) override;

    // Members
};

inline void ConnectComponentsCommand::execute(Circuit& circuit)
{
}

inline void ConnectComponentsCommand::undo(Circuit& circuit)
{
}

inline void ConnectComponentsCommand::redo(Circuit& circuit)
{
}
