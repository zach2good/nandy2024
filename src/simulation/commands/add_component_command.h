#pragma once

#include "simulation/circuit.h"
#include "simulation/commands/command.h"

class AddComponentCommand : public Command
{
public:
    AddComponentCommand(f32 x, f32 y, std::string payload);
    ~AddComponentCommand();

    void execute(Circuit& circuit) override;
    void undo(Circuit& circuit) override;
    void redo(Circuit& circuit) override;

    // Members
    f32         x;
    f32         y;
    std::string payload;
};

inline AddComponentCommand::AddComponentCommand(f32 x, f32 y, std::string payload)
: x(x)
, y(y)
, payload(std::move(payload))
{
}

inline AddComponentCommand::~AddComponentCommand()
{
}

inline void AddComponentCommand::execute(Circuit& circuit)
{
    if (payload == "NAND")
    {
        circuit.addNAND({ x, y });
    }
}

inline void AddComponentCommand::undo(Circuit& circuit)
{
}

inline void AddComponentCommand::redo(Circuit& circuit)
{
}
