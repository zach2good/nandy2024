#pragma once

#include "ui/actions/action.h"

#include <memory>

class CanvasController final
{
public:
    CanvasController();
    ~CanvasController();

    void handleCanvasAction(std::unique_ptr<Action> action);
};

inline CanvasController::CanvasController()
{
}

inline CanvasController::~CanvasController()
{
}

inline void CanvasController::handleCanvasAction(std::unique_ptr<Action> action)
{
    spdlog::info("Handling canvas action: {}", action->getName());
}
