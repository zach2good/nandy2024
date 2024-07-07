#pragma once

#include "types.h"
#include "ui/events/event.h"

#include <fmt/format.h>

struct UIDragEndedEvent final : public Event
{
    UIDragEndedEvent();
    ~UIDragEndedEvent();

    auto getName() const -> std::string override
    {
        return "UIDragEndedEvent";
    }

    auto toString() const -> std::string override
    {
        return fmt::format("UIDragEndedEvent");
    }

private:
};

inline UIDragEndedEvent::UIDragEndedEvent()
{
}

inline UIDragEndedEvent::~UIDragEndedEvent()
{
}
