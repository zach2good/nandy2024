#pragma once

#include "types.h"
#include "ui/actions/action.h"

#include <fmt/format.h>

struct UIKeypressAction final : public Action
{
    UIKeypressAction(u32 val);
    ~UIKeypressAction();

    auto getName() const -> std::string override
    {
        return fmt::format("UIKeypressAction: val={}", val);
    }

private:
    u32 val;
};

inline UIKeypressAction::UIKeypressAction(u32 val)
: val(val)
{
}

inline UIKeypressAction::~UIKeypressAction()
{
}
