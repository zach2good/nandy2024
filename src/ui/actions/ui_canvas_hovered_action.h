#pragma once

#include "types.h"
#include "ui/actions/action.h"

struct UICanvasHoveredAction final : public Action
{
    UICanvasHoveredAction();
    ~UICanvasHoveredAction();

    auto getName() const -> std::string override
    {
        return "UICanvasHoveredAction";
    }

    auto toString() const -> std::string override
    {
        return "UICanvasHoveredAction";
    }
};

inline UICanvasHoveredAction::UICanvasHoveredAction()
{
}

inline UICanvasHoveredAction::~UICanvasHoveredAction()
{
}
