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
    Position m_CursorDelta;

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
            m_CursorDelta    = Position(moveAction->dx, moveAction->dy);

            if (!m_HasDragged)
            {
                events.emplace_back(std::make_unique<UIDragStartedEvent>());
                m_HasDragged = true;
            }

            if (m_IsDragging)
            {
                events.emplace_back(std::make_unique<UIDragUpdateEvent>());
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
    }

    /*
    const auto cursorX = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
    const auto cursorY = Config::kCanvasHeight + ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY();
    */

    return events;
}
