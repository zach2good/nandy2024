#pragma once

#include "ui/events/event.h"

#include <memory>

class CanvasController final
{
public:
    CanvasController();
    ~CanvasController();

    void handleCanvasEvent(std::unique_ptr<Event> event);
};

inline CanvasController::CanvasController()
{
}

inline CanvasController::~CanvasController()
{
}

inline void CanvasController::handleCanvasEvent(std::unique_ptr<Event> event)
{
    spdlog::info("CanvasController::handleCanvasEvent: event={}", event->toString());
}
