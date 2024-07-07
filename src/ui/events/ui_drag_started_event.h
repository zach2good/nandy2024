#pragma once

#include "types.h"
#include "ui/events/event.h"

#include <fmt/format.h>

struct UIDragStartedEvent final : public Event
{
    UIDragStartedEvent();
    ~UIDragStartedEvent();

    auto getName() const -> std::string override
    {
        return "UIDragStartedEvent";
    }

    auto toString() const -> std::string override
    {
        return fmt::format("UIDragStartedEvent");
    }

private:
};

inline UIDragStartedEvent::UIDragStartedEvent()
{
}

inline UIDragStartedEvent::~UIDragStartedEvent()
{
}
