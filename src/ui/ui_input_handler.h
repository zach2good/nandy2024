#pragma once

#include <vector>

#include "config.h"
#include "ui/actions/action.h"
#include "ui/actions/canvas_component_hovered_action.h"
#include "ui/canvas_view_model.h"

class UIInputHandler final
{
public:
    UIInputHandler();
    ~UIInputHandler();

    auto handleInput(CanvasViewModel* canvasViewModel) -> std::vector<std::unique_ptr<Action>>;
};

inline UIInputHandler::UIInputHandler()
{
}

inline UIInputHandler::~UIInputHandler()
{
}

inline auto UIInputHandler::handleInput(CanvasViewModel* canvasViewModel) -> std::vector<std::unique_ptr<Action>>
{
    std::vector<std::unique_ptr<Action>> actions;

    /*
    const auto cursorX = ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
    const auto cursorY = Config::kCanvasHeight + ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY();
    const auto offsetX = 0;
    const auto offsetY = 0;
    const auto zoom    = 1;

    for (auto& nand : canvasViewModel->m_NANDs)
    {
        if (cursorX > offsetX + nand.position.x * zoom &&
            cursorX < offsetX + nand.position.x * zoom + nand.size.width * zoom &&
            cursorY > offsetY + nand.position.y * zoom &&
            cursorY < offsetY + nand.position.y * zoom + nand.size.height * zoom)
        {
        }
    }
    */
    // actions.push_back(std::make_unique<CanvasComponentHoveredAction>());

    return actions;
}
