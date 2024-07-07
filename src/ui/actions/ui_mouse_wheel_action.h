#pragma once

#include "types.h"
#include "ui/actions/action.h"

#include <fmt/format.h>

struct UIMouseWheelAction final : public Action
{
    UIMouseWheelAction(f32 val);
    ~UIMouseWheelAction();

    auto getName() const -> std::string override
    {
        return "UIMouseWheelAction";
    }

    auto toString() const -> std::string override
    {
        return fmt::format("UIMouseWheelAction: val={}", val);
    }

private:
    f32 val;
};

inline UIMouseWheelAction::UIMouseWheelAction(f32 val)
: val(val)
{
}

inline UIMouseWheelAction::~UIMouseWheelAction()
{
}
