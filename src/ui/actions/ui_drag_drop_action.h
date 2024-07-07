#pragma once

#include "types.h"
#include "ui/actions/action.h"

#include <fmt/format.h>
#include <string>

struct UIDragDropAction final : public Action
{
    UIDragDropAction(f32 x, f32 y, std::string payload);
    ~UIDragDropAction();

    auto getName() const -> std::string override
    {
        return "UIDragDropAction";
    }

    auto toString() const -> std::string override
    {
        return fmt::format("UIDragDropAction: x={}, y={}, payload={}", x, y, payload);
    }

private:
    f32         x;
    f32         y;
    std::string payload;
};

inline UIDragDropAction::UIDragDropAction(f32 x, f32 y, std::string payload)
: x(x)
, y(y)
, payload(std::move(payload))
{
}

inline UIDragDropAction::~UIDragDropAction()
{
}
