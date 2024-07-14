#pragma once

#include "simulation/circuit.h"
#include "ui/renderers/window_renderer.h"

#include <memory>

class CanvasViewModel final
{
public:
    static std::unique_ptr<CanvasViewModel> create(Circuit& circuit);

    void draw(WindowRenderer* renderer);
    void update();

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

    // Base Components
    std::vector<NANDViewModel> m_NANDs;
    std::vector<NodeViewModel> m_Nodes;
    std::vector<WireViewModel> m_Wires;

    Delta m_Offset = { 0.0f, 0.0f };
    f32   m_Zoom   = 1.0f;

    Circuit& m_Circuit;

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
    // TODO: Does it hide intent if I clear the canvas in here instead of outside?

    for (auto& nand : m_NANDs)
    {
        renderer->setColour(Colour::White);
        renderer->drawNAND({ m_Offset.dx + nand.position.x * m_Zoom, m_Offset.dy + nand.position.y * m_Zoom }, { nand.size.width * m_Zoom, nand.size.height * m_Zoom }, nand.facing);
    }

    for (auto& node : m_Nodes)
    {
        // renderer->setColour(Colour::White);
        // renderer->drawRectangle({ node.position.x * m_Zoom, node.position.y * m_Zoom }, { node.size.width * m_Zoom, node.size.height * m_Zoom });
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
: m_Circuit(circuit)
{
}

inline void CanvasViewModel::update()
{
    u64 ids = 0;
    for (auto& component : m_Circuit.components)
    {
        if (auto* nandGate = dynamic_cast<NandGate*>(component))
        {
            m_NANDs.push_back({ ids++, nandGate->position, { 100, 100 }, Facing::Right });
        }
    }
}
