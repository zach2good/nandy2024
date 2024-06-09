#pragma once

#include "types.h"
#include "ui/actions/action.h"

#include <fmt/format.h>

struct UIMouseMovedAction final : public Action
{
    UIMouseMovedAction(f32 x, f32 y, f32 dx, f32 dy);
    ~UIMouseMovedAction();

    auto getName() const -> std::string override
    {
        return fmt::format("UIMouseMovedAction: x={}, y={}, dx={}, dy={}", x, y, dx, dy);
    }

private:
    f32 x;
    f32 y;
    f32 dx;
    f32 dy;
};

inline UIMouseMovedAction::UIMouseMovedAction(f32 x, f32 y, f32 dx, f32 dy)
: x(x)
, y(y)
, dx(dx)
, dy(dy)
{
}

inline UIMouseMovedAction::~UIMouseMovedAction()
{
}
