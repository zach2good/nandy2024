#pragma once

#include "simulation/circuit.h"
#include "ui/renderers/window_renderer.h"

#include <memory>

class CanvasViewModel final
{
public:
    static std::unique_ptr<CanvasViewModel> create(Circuit& circuit);

    void draw(WindowRenderer* renderer);

    struct NANDViewModel final
    {
        u64      id;
        Position position;
        Size     size;
        Facing   facing;

        // TODO: Input/output states
        // TODO: Connections: maybe do through wires?

        auto toString() const -> std::string
        {
            return fmt::format("NANDViewModel: id={}, position={}, size={}, facing={}", id, ::toString(position), ::toString(size), ::toString(facing));
        }
    };

    struct NodeViewModel final
    {
        u64      id;
        Position position;
        Size     size;
    };

    struct WireViewModel final
    {
        u64      id;
        Position start;
        Position end;
    };

    std::vector<NANDViewModel> m_NANDs;
    std::vector<NodeViewModel> m_Nodes;
    std::vector<WireViewModel> m_Wires;

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

    for (auto& node : m_Nodes)
    {
        renderer->setColour(Colour::White);
        renderer->drawRectangle(node.position, node.size);
    }

    for (auto& wire : m_Wires)
    {
        renderer->setColour(Colour::White);
        renderer->drawLine(wire.start, wire.end);
    }

    // TODO: Selection box
    // renderer->setColour(Colour::White);
    // renderer->drawRectangle(node.position, node.size);

    // TODO: Labels/Notes
}

// TODO: Drawing and creation should be seperate?
inline CanvasViewModel::CanvasViewModel(Circuit& circuit)
{
    // TODO: This is faked for now while I work on the UI
    m_NANDs.push_back({ 1, { 100, 100 }, { 100, 100 }, Facing::Right });
    m_NANDs.push_back({ 2, { 250, 100 }, { 100, 100 }, Facing::Right });

    m_Wires.push_back({ 1, { 350, 350 }, { 450, 450 } });
}
