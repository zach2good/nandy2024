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
};

inline UIMouseDownAction::UIMouseDownAction()
{
}

inline UIMouseDownAction::~UIMouseDownAction()
{
}
