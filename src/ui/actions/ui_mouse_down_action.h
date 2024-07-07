#pragma once

#include "types.h"
#include "ui/actions/action.h"

struct UIMouseDownAction final : public Action
{
    UIMouseDownAction();
    ~UIMouseDownAction();

    auto getName() const -> std::string override
    {
        return "UIMouseDownAction";
    }

    auto toString() const -> std::string override
    {
        return fmt::format("UIMouseDownAction");
    }
};

inline UIMouseDownAction::UIMouseDownAction()
{
}

inline UIMouseDownAction::~UIMouseDownAction()
{
}
