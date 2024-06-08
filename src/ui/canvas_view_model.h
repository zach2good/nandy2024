#pragma once

#include "simulation/circuit.h"
#include "ui/renderers/window_renderer.h"

#include <memory>

class CanvasViewModel
{
public:
    static std::unique_ptr<CanvasViewModel> create(Circuit& circuit);

    void draw(WindowRenderer* renderer);

private:
    CanvasViewModel(Circuit& circuit);

    // TODO: Don't use ComponentId, or anything that needs logicSim.
    //     : These should be self-contained primitives. Once created,
    //     : CanvasViewModel should not need to interact with LogicSim
    //     : in order to render.
    // std::vector<ComponentId> m_VisibleComponentIds;
    // std::vector<ComponentId> m_SelectedComponentIds;
    // ComponentId              m_HoveredComponentId;
};

inline std::unique_ptr<CanvasViewModel> CanvasViewModel::create(Circuit& circuit)
{
    return std::move(std::unique_ptr<CanvasViewModel>(new CanvasViewModel(circuit)));
}

inline void CanvasViewModel::draw(WindowRenderer* renderer)
{
    renderer->setColour(Colour::Yellow);
    renderer->drawNAND({ 100, 100 }, { 100, 100 }, Facing::Right);

    renderer->setColour(Colour::Magenta);
    renderer->drawNAND({ 100, 200 }, { 100, 100 }, Facing::Left);

    renderer->setColour(Colour::Cyan);
    renderer->drawNAND({ 200, 100 }, { 100, 100 }, Facing::Up);

    renderer->setColour(Colour::LightGrey);
    renderer->drawNAND({ 200, 200 }, { 100, 100 }, Facing::Down);
}

inline CanvasViewModel::CanvasViewModel(Circuit& circuit)
{
    // TODO: Make viewmodel from circuit
}
