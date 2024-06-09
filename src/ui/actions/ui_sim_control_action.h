#pragma once

#include "types.h"
#include "ui/actions/action.h"

#include <fmt/format.h>

enum class SimControl
{
    Step,
    Run,
    Stop,
    Reset,
};

std::string to_string(SimControl control)
{
    switch (control)
    {
        case SimControl::Step:
            return "Step";
        case SimControl::Run:
            return "Run";
        case SimControl::Stop:
            return "Stop";
        case SimControl::Reset:
            return "Reset";
        default:
            return "Unknown";
    }
}

struct UISimControlAction final : public Action
{
    UISimControlAction(SimControl control);
    ~UISimControlAction();

    auto getName() const -> std::string override
    {
        return fmt::format("UISimControlAction: val={}", to_string(control));
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
