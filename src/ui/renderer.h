#pragma once

#include "common.h"

#include "logic/logic_sim.h"

#include <optional>

class Renderer
{
public:
    Renderer(std::string const& title, std::size_t width, std::size_t height);
    ~Renderer();

    // Higher level API
    void handleInput(SDL_Event const& event);
    void clear();
    void draw(LogicSim& logicSim);
    void present();

    usize m_Width;
    usize m_Height;
    f32   m_OffsetX         = 0.0f;
    f32   m_OffsetY         = 0.0f;
    f32   m_Zoom            = 1.0f;
    float m_CursorX         = 0.0f;
    float m_CursorY         = 0.0f;
    bool  m_IsCanvasHovered = false;

    f32 m_MouseDragStartX       = 0.0f;
    f32 m_MouseDragStartY       = 0.0f;
    f32 m_MouseDragEndX         = 0.0f;
    f32 m_MouseDragEndY         = 0.0f;

private:
    // Drawing primitives on the canvas
    void drawPrimitiveLine(float x1, float y1, float x2, float y2);
    void drawPrimitiveRectangle(float x, float y, float w, float h);
    void drawPrimitiveCircle(int centerX, int centerY, int radius);
    void drawPrimitiveNAND(float x, float y, float w, float h, bool a, bool b, bool o);
    void drawUI(LogicSim& logicSim);

    // TODO: Each UI state needs to be encapsulated in it's own class so it can have it's own storage and logic
    enum class UIState
    {
        // Base state
        None,

        // UI interaction states
        UIDragging,

        // Canvas interaction states
        CanvasPanning,
        CanvasMovingComponent,
        CanvasConnectingComponents,
        CanvasSelecting,
    } m_UIState = UIState::None;

    std::string UIStateToString(UIState state)
    {
        switch (state)
        {
            case UIState::None: return "None";
            case UIState::UIDragging: return "UIDragging";
            case UIState::CanvasPanning: return "CanvasPanning";
            case UIState::CanvasMovingComponent: return "CanvasMovingComponent";
            case UIState::CanvasConnectingComponents: return "CanvasConnectingComponents";
            case UIState::CanvasSelecting: return "CanvasSelecting";
        }
        return "Unknown";
    }

    // All states (these are cleared every redraw)
    std::optional<std::reference_wrapper<Component>> m_ComponentUnderMouse;

    // CanvasMovingComponent
    std::optional<std::reference_wrapper<Component>> m_ComponentBeingMoved;

    // CanvasConnectingComponents
    std::optional<std::reference_wrapper<Component>> m_ComponentConnectionSource;
    std::optional<std::reference_wrapper<Component>> m_ComponentConnectionTarget;

    // SDL2
    SDL_Window*   m_Window;
    SDL_Renderer* m_Renderer;
    SDL_Texture*  m_CanvasTexture;
};
