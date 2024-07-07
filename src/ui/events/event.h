#pragma once

#include <string>

struct Event
{
    virtual ~Event() = default;

    virtual auto getName() const -> std::string = 0;
    virtual auto toString() const -> std::string = 0;
};
