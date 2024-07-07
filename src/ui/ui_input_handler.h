#pragma once

#include <vector>

#include "config.h"

#include "ui/actions/action.h"
#include "ui/canvas_view_model.h"

#include "ui/events/event.h"
#include "ui/events/ui_drag_ended_event.h"
#include "ui/events/ui_drag_started_event.h"
#include "ui/events/ui_drag_update_event.h"
#include "ui/events/ui_mouse_click_event.h"

class UIInputHandler final
{
public:
    UIInputHandler();
    ~UIInputHandler();

    auto handleInput(CanvasViewModel* canvasViewModel, std::vector<std::unique_ptr<Action>> actions) -> std::vector<std::unique_ptr<Event>>;

private:
    Position m_CursorPosition;
    Delta    m_CursorDelta;

    bool     m_IsDragging = false;
    bool     m_HasDragged = false;
    Position m_DragStart;
    Position m_DragEnd;
};

inline UIInputHandler::UIInputHandler()
{
}

inline UIInputHandler::~UIInputHandler()
{
}

inline auto UIInputHandler::handleInput(CanvasViewModel* canvasViewModel, std::vector<std::unique_ptr<Action>> actions) -> std::vector<std::unique_ptr<Event>>
{
    std::vector<std::unique_ptr<Event>> events;

    for (auto& action : actions)
    {
        // spdlog::info("UIInputHandler::handleInput: action={}", action->getName());

        // TODO: Do the type lookup without using the name, lol!

        if (action->getName() == "UIMouseDownAction" && !m_IsDragging)
        {
            auto dragAction = static_cast<UIMouseDownAction*>(action.get());
            m_IsDragging    = true;
            m_DragStart     = m_CursorPosition;
        }

        if (action->getName() == "UIMouseMovedAction")
        {
            auto moveAction  = static_cast<UIMouseMovedAction*>(action.get());
            m_CursorPosition = Position(moveAction->x, moveAction->y);
            m_CursorDelta    = Delta(moveAction->dx, moveAction->dy);

            if (m_IsDragging)
            {
                if (!m_HasDragged)
                {
                    events.emplace_back(std::make_unique<UIDragStartedEvent>());
                    m_HasDragged = true;
                }

                events.emplace_back(std::make_unique<UIDragUpdateEvent>());
            }

            // If the mouse has moved, we have to check the visible components on the canvas
            // to see if we're hovering over any of them.
            // TODO: Culling & caching
            for (auto& nandViewModel : canvasViewModel->m_NANDs)
            {
                // TODO: Handle zoom and pan
                if (m_CursorPosition.x >= nandViewModel.position.x &&
                    m_CursorPosition.x <= nandViewModel.position.x + nandViewModel.size.width &&
                    m_CursorPosition.y >= nandViewModel.position.y &&
                    m_CursorPosition.y <= nandViewModel.position.y + nandViewModel.size.height)
                {
                    // Component hovered
                    spdlog::info("UIInputHandler::handleInput: nandViewModel={}", nandViewModel.toString());
                }
            }
        }

        if (action->getName() == "UIMouseUpAction" && m_IsDragging)
        {
            m_DragEnd = m_CursorPosition;
            if (m_DragStart.x != m_DragEnd.x || m_DragStart.y != m_DragEnd.y)
            {
                events.emplace_back(std::make_unique<UIDragEndedEvent>());
            }
            else
            {
                events.emplace_back(std::make_unique<UIMouseClickEvent>());
            }

            m_IsDragging = false;
            m_HasDragged = false;
        }

        if (action->getName() == "UICanvasHoveredAction")
        {
            m_IsDragging = false;
        }

        if (action->getName() == "UIDragDropAction")
        {
            auto dragDropAction = static_cast<UIDragDropAction*>(action.get());
            spdlog::info("UIInputHandler::handleInput: {}", dragDropAction->toString());
        }

        if (action->getName() == "UISimControlAction")
        {
            auto simControlAction = static_cast<UISimControlAction*>(action.get());
            spdlog::info("UIInputHandler::handleInput: {}", simControlAction->toString());
        }
    }

    return events;
}
