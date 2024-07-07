#pragma once

#include "types.h"
#include "ui/actions/action.h"

#include <fmt/format.h>

struct UISimControlAction final : public Action
{
    UISimControlAction(SimControl control);
    ~UISimControlAction();

    auto getName() const -> std::string override
    {
        return "UISimControlAction";
    }

    auto toString() const -> std::string override
    {
        return fmt::format("UISimControlAction: val={}", ::toString(control));
    }

private:
    SimControl control;
};

inline UISimControlAction::UISimControlAction(SimControl control)
: control(control)
{
}

inline UISimControlAction::~UISimControlAction()
{
}
