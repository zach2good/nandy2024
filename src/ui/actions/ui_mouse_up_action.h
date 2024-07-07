#pragma once

#include "types.h"
#include "ui/actions/action.h"

struct UIMouseUpAction final : public Action
{
    UIMouseUpAction();
    ~UIMouseUpAction();

    auto getName() const -> std::string override
    {
        return "UIMouseUpAction";
    }

    auto toString() const -> std::string override
    {
        return fmt::format("UIMouseUpAction");
    }
};

inline UIMouseUpAction::UIMouseUpAction()
{
}

inline UIMouseUpAction::~UIMouseUpAction()
{
}
