#pragma once

#include "types.h"
#include "ui/events/event.h"

#include <fmt/format.h>

struct UIDragDropEvent final : public Event
{
    UIDragDropEvent(f32 x, f32 y, std::string payload);
    ~UIDragDropEvent();

    auto getName() const -> std::string override
    {
        return "UIDragDropEvent";
    }

    auto toString() const -> std::string override
    {
        return fmt::format("UIDragDropEvent: x={}, y={}, payload={}", x, y, payload);
    }

    // private:
    f32         x;
    f32         y;
    std::string payload;
};

inline UIDragDropEvent::UIDragDropEvent(f32 x, f32 y, std::string payload)
: x(x)
, y(y)
, payload(std::move(payload))
{
}

inline UIDragDropEvent::~UIDragDropEvent()
{
}
