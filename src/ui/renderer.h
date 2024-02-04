#pragma once

#include "common.h"

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

    std::size_t m_Width;
    std::size_t m_Height;
    f32         m_OffsetX = 0.0f;
    f32         m_OffsetY = 0.0f;
    f32         m_Zoom    = 1.0f;
    float       m_CursorX = 0.0f;
    float       m_CursorY = 0.0f;

private:
    // Drawing primitives on the canvas
    void drawPrimitiveLine(float x1, float y1, float x2, float y2);
    void drawPrimitiveRectangle(float x, float y, float w, float h);
    void drawPrimitiveCircle(int centerX, int centerY, int radius);
    void drawPrimitiveNAND(float x, float y, float w, float h, bool a, bool b, bool o);
    void drawUI(LogicSim& logicSim);

    // Handling interactions with the canvas
    // If mouse is over the canvas, etc.
    // Dragging, zooming, delta pan, mouse input, etc.

    SDL_Window*   m_Window;
    SDL_Renderer* m_Renderer;
    SDL_Texture*  m_CanvasTexture;
};
