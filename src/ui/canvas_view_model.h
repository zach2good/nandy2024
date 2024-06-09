#pragma once

#include "simulation/circuit.h"
#include "ui/renderers/window_renderer.h"

#include <memory>

class CanvasViewModel final
{
public:
    static std::unique_ptr<CanvasViewModel> create(Circuit& circuit);

    void draw(WindowRenderer* renderer);

    struct NANDViewModel
    {
        u64      id;
        Position position;
        Size     size;
        Facing   facing;

        // TODO: Input/output states
        // TODO: Connections: maybe do through wires?
    };

    std::vector<NANDViewModel> m_NANDs;

private:
    CanvasViewModel(Circuit& circuit);
};

inline std::unique_ptr<CanvasViewModel> CanvasViewModel::create(Circuit& circuit)
{
    return std::move(std::unique_ptr<CanvasViewModel>(new CanvasViewModel(circuit)));
}

// TODO: Drawing and creation should be seperate?
inline void CanvasViewModel::draw(WindowRenderer* renderer)
{
    for (auto& nand : m_NANDs)
    {
        renderer->setColour(Colour::White);
        renderer->drawNAND(nand.position, nand.size, nand.facing);
    }

    // TODO: Nodes

    // TODO: Wires

    // TODO: Selection box

    // TODO: Labels/Notes
}

// TODO: Drawing and creation should be seperate?
inline CanvasViewModel::CanvasViewModel(Circuit& circuit)
{
    // TODO: This is faked for now while I work on the UI
    m_NANDs.push_back({ 1, { 100, 100 }, { 100, 100 }, Facing::Right });
    m_NANDs.push_back({ 2, { 250, 100 }, { 100, 100 }, Facing::Right });
}
