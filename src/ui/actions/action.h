#pragma once

#include <string>

// An action is a low level message that is sent from Dear ImGui and the renderer to
// allow us to build up more complex stateful behaviours.
// User inputs (mouse clicks, key presses) are captured as actions.
struct Action
{
    virtual ~Action() = default;

    virtual auto getName() const -> std::string = 0;
    virtual auto toString() const -> std::string = 0;
};
