#pragma once

#include "types.h"
#include "ui/actions/action.h"

struct CanvasComponentHoveredAction final : public Action
{
    CanvasComponentHoveredAction();
    ~CanvasComponentHoveredAction();

    auto getName() const -> std::string override
    {
        return "CanvasComponentHoveredAction";
    }
};

inline CanvasComponentHoveredAction::CanvasComponentHoveredAction()
{
}

inline CanvasComponentHoveredAction::~CanvasComponentHoveredAction()
{
}
