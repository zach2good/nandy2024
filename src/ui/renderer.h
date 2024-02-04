#pragma once

#include "common.h"

class Renderer
{
public:
    Renderer(std::string const& title, std::size_t width, std::size_t height);
    ~Renderer();

    void handleInput(SDL_Event const& event);
    void clear();
    void draw(LogicSim& logicSim);
    void present();

    std::size_t m_Width;
    std::size_t m_Height;
    f32         m_OffsetX = 0.0f;
    f32         m_OffsetY = 0.0f;
    f32         m_Zoom    = 1.0f;

private:
    void drawPrimitiveNAND(float x, float y, float w, float h, bool a, bool b, bool o);

    SDL_Window*   m_Window;
    SDL_Renderer* m_Renderer;
    SDL_Texture*  m_CanvasTexture;
};
