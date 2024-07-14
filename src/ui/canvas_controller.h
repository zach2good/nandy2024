#pragma once

#include "ui/events/event.h"

#include "simulation/commands/add_component_command.h"
#include "simulation/commands/command.h"

#include "ui/events/ui_drag_drop_event.h"

#include <memory>

class CanvasController final
{
public:
    CanvasController();
    ~CanvasController();

    auto handleCanvasEvent(std::unique_ptr<Event> event) -> std::vector<std::unique_ptr<Command>>;
};

inline CanvasController::CanvasController()
{
}

inline CanvasController::~CanvasController()
{
}

inline auto CanvasController::handleCanvasEvent(std::unique_ptr<Event> event) -> std::vector<std::unique_ptr<Command>>
{
    spdlog::info("CanvasController::handleCanvasEvent: event={}", event->toString());

    std::vector<std::unique_ptr<Command>> commands;

    if (event->getName() == "UIDragDropEvent")
    {
        auto* dragDropEvent = static_cast<UIDragDropEvent*>(event.get());
        commands.push_back(std::make_unique<AddComponentCommand>(dragDropEvent->x, dragDropEvent->y, dragDropEvent->payload));
    }

    return commands;
}
