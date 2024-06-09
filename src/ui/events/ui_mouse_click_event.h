#pragma once

#include "types.h"
#include "ui/events/event.h"

#include <fmt/format.h>

struct UIMouseClickEvent final : public Event
{
    UIMouseClickEvent();
    ~UIMouseClickEvent();

    auto getName() const -> std::string override
    {
        return "UIMouseClickEvent";
    }

private:
};

inline UIMouseClickEvent::UIMouseClickEvent()
{
}

inline UIMouseClickEvent::~UIMouseClickEvent()
{
}
