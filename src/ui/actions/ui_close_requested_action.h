#pragma once

#include "types.h"
#include "ui/actions/action.h"

struct UICloseRequestedAction final : public Action
{
    UICloseRequestedAction();
    ~UICloseRequestedAction();

    auto getName() const -> std::string override
    {
        return "UICloseRequestedAction";
    }
};

inline UICloseRequestedAction::UICloseRequestedAction()
{
}

inline UICloseRequestedAction::~UICloseRequestedAction()
{
}
