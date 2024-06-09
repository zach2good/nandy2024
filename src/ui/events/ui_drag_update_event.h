#pragma once

#include "types.h"
#include "ui/events/event.h"

#include <fmt/format.h>

struct UIDragUpdateEvent final : public Event
{
    UIDragUpdateEvent();
    ~UIDragUpdateEvent();

    auto getName() const -> std::string override
    {
        return "UIDragUpdateEvent";
    }

private:
};

inline UIDragUpdateEvent::UIDragUpdateEvent()
{
}

inline UIDragUpdateEvent::~UIDragUpdateEvent()
{
}
