#pragma once

#include <string>

// An event is a higher-level message made up of and triggered by actions and combined state to capture
// more complex user interactions.
// Actions are processed to generate events that represent higher-level interactions.
struct Event
{
    virtual ~Event() = default;

    virtual auto getName() const -> std::string = 0;
    virtual auto toString() const -> std::string = 0;
};
