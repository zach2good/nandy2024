#pragma once

#include <string>

struct Action
{
    virtual ~Action() = default;

    virtual auto getName() const -> std::string = 0;
};
