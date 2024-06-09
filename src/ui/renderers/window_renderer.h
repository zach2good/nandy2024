#pragma once

#include "config.h"
#include "types.h"

#include <memory>
#include <optional>
#include <stack>
#include <vector>

#include <SDL.h>
#include <imgui.h>

class WindowRenderer final
{
public:
    WindowRenderer(std::string const& title, Size const& size);
    ~WindowRenderer();

    void handleEvent(SDL_Event const& event);
    void clear();
    void draw();
    void present();

    void drawLine(const Position& start, const Position& end);
    void drawRectangle(const Position& topLeft, const Size& size);
    void drawCircle(const Position& center, f64 radius);
    void drawNAND(const Position& topLeft, const Size& size, Facing facing);

    void setColour(Colour colour);
    void setDrawToCanvas();
    void setDrawToScreen();

    auto sdlWindow() -> SDL_Window*;
    auto sdlRenderer() -> SDL_Renderer*;
    auto sdlCanvasTexture() -> SDL_Texture*;

private:
    // SDL2
    SDL_Window*   m_Window;
    SDL_Renderer* m_Renderer;
    SDL_Texture*  m_CanvasTexture;
};

inline WindowRenderer::WindowRenderer(std::string const& title, Size const& size)
{
    // Setup SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Setup SDL Window
    m_Window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        size.width,
        size.height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
    if (m_Window == nullptr)
    {
        spdlog::error("Could not create window: {}", SDL_GetError());
        std::exit(-1);
    }

    // Setup SDL Renderer
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (m_Renderer == nullptr)
    {
        spdlog::error("Error creating SDL_Renderer: {}", SDL_GetError());
        std::exit(-1);
    }

    m_CanvasTexture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Config::kCanvasWidth, Config::kCanvasHeight);
}

inline WindowRenderer::~WindowRenderer()
{
    SDL_DestroyTexture(m_CanvasTexture);
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

inline void WindowRenderer::handleEvent(SDL_Event const& event)
{
}

inline void WindowRenderer::clear()
{
    SDL_RenderClear(m_Renderer);
    SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
}

inline void WindowRenderer::draw()
{
}

inline void WindowRenderer::present()
{
    SDL_RenderPresent(m_Renderer);
}

inline void WindowRenderer::drawLine(const Position& start, const Position& end)
{
    SDL_RenderDrawLine(m_Renderer, start.x, start.y, end.x, end.y);
}

inline void WindowRenderer::drawRectangle(const Position& topLeft, const Size& size)
{
    SDL_Rect rect;
    rect.x = topLeft.x;
    rect.y = topLeft.y;
    rect.w = size.width;
    rect.h = size.height;
    SDL_RenderDrawRect(m_Renderer, &rect);
}

inline void WindowRenderer::drawCircle(const Position& center, f64 radius)
{
    constexpr int kSegments = 16;
    for (int i = 0; i < kSegments; ++i)
    {
        const f64 angle1 = 2.0 * M_PI * static_cast<f64>(i) / static_cast<f64>(kSegments);
        const f64 angle2 = 2.0 * M_PI * static_cast<f64>(i + 1) / static_cast<f64>(kSegments);

        const f64 x1 = center.x + radius * std::cos(angle1);
        const f64 y1 = center.y + radius * std::sin(angle1);
        const f64 x2 = center.x + radius * std::cos(angle2);
        const f64 y2 = center.y + radius * std::sin(angle2);

        SDL_RenderDrawLine(m_Renderer, x1, y1, x2, y2);
    }
}

inline void WindowRenderer::drawNAND(const Position& topLeft, const Size& size, Facing facing)
{
    switch (facing)
    {
        case Facing::Right:
        {
            // Legs
            drawLine({ topLeft.x + size.width * 0.0f, topLeft.y + size.height * 0.3f }, { topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.3f });
            drawLine({ topLeft.x + size.width * 0.0f, topLeft.y + size.height * 0.7f }, { topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.7f });
            drawLine({ topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.5f }, { topLeft.x + size.width * 1.0f, topLeft.y + size.height * 0.5f });

            // Body
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.2f }, { topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.8f });
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.2f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.2f });
            drawLine({ topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.2f }, { topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.3f });
            drawLine({ topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.3f }, { topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.5f });
            drawLine({ topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.7f }, { topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.5f });
            drawLine({ topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.8f }, { topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.7f });
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.8f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.8f });

            // Draw circle
            drawCircle({ topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.5f }, size.width * 0.05f);
        }
        break;
        case Facing::Down:
        {
            // Legs
            drawLine({ topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.0f }, { topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.2f });
            drawLine({ topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.0f }, { topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.2f });
            drawLine({ topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.8f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 1.0f });

            // Body
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.2f }, { topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.2f });
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.2f }, { topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.5f });
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.5f }, { topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.7f });
            drawLine({ topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.7f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.8f });
            drawLine({ topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.7f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.8f });
            drawLine({ topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.5f }, { topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.2f });
            drawLine({ topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.5f }, { topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.7f });

            // Draw circle
            drawCircle({ topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.8f }, size.width * 0.05f);
        }
        break;
        case Facing::Left:
        {
            // Legs
            drawLine({ topLeft.x + size.width * 1.0f, topLeft.y + size.height * 0.3f }, { topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.3f });
            drawLine({ topLeft.x + size.width * 1.0f, topLeft.y + size.height * 0.7f }, { topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.7f });
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.5f }, { topLeft.x + size.width * 0.0f, topLeft.y + size.height * 0.5f });

            // Body
            drawLine({ topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.2f }, { topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.8f });
            drawLine({ topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.2f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.2f });
            drawLine({ topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.2f }, { topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.3f });
            drawLine({ topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.3f }, { topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.5f });
            drawLine({ topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.7f }, { topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.5f });
            drawLine({ topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.8f }, { topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.7f });
            drawLine({ topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.8f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.8f });

            // Draw circle
            drawCircle({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.5f }, size.width * 0.05f);
        }
        break;
        case Facing::Up:
        {
            // Legs
            drawLine({ topLeft.x + size.width * 0.3f, topLeft.y + size.height * 1.0f }, { topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.8f });
            drawLine({ topLeft.x + size.width * 0.7f, topLeft.y + size.height * 1.0f }, { topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.8f });
            drawLine({ topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.2f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.0f });

            // Body
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.8f }, { topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.8f });
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.8f }, { topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.5f });
            drawLine({ topLeft.x + size.width * 0.2f, topLeft.y + size.height * 0.5f }, { topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.3f });
            drawLine({ topLeft.x + size.width * 0.3f, topLeft.y + size.height * 0.3f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.2f });
            drawLine({ topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.3f }, { topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.2f });
            drawLine({ topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.5f }, { topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.8f });
            drawLine({ topLeft.x + size.width * 0.8f, topLeft.y + size.height * 0.5f }, { topLeft.x + size.width * 0.7f, topLeft.y + size.height * 0.3f });

            // Draw circle
            drawCircle({ topLeft.x + size.width * 0.5f, topLeft.y + size.height * 0.2f }, size.width * 0.05f);
        }
        break;
    }
}

inline void WindowRenderer::setColour(Colour colour)
{
    switch (colour)
    {
        case Colour::Red:
            SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
            break;
        case Colour::Green:
            SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 255);
            break;
        case Colour::Blue:
            SDL_SetRenderDrawColor(m_Renderer, 0, 0, 255, 255);
            break;
        case Colour::Yellow:
            SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
            break;
        case Colour::Cyan:
            SDL_SetRenderDrawColor(m_Renderer, 0, 255, 255, 255);
            break;
        case Colour::Magenta:
            SDL_SetRenderDrawColor(m_Renderer, 255, 0, 255, 255);
            break;
        case Colour::White:
            SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);
            break;
        case Colour::Black:
            SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
            break;
        case Colour::Grey:
            SDL_SetRenderDrawColor(m_Renderer, 128, 128, 128, 255);
            break;
        case Colour::LightGrey:
            SDL_SetRenderDrawColor(m_Renderer, 192, 192, 192, 255);
            break;
        case Colour::DarkGrey:
            SDL_SetRenderDrawColor(m_Renderer, 64, 64, 64, 255);
            break;
    }
}

inline void WindowRenderer::setDrawToCanvas()
{
    SDL_SetRenderTarget(m_Renderer, m_CanvasTexture);
}

inline void WindowRenderer::setDrawToScreen()
{
    SDL_SetRenderTarget(m_Renderer, nullptr);
}

inline auto WindowRenderer::sdlWindow() -> SDL_Window*
{
    return m_Window;
}

inline auto WindowRenderer::sdlRenderer() -> SDL_Renderer*
{
    return m_Renderer;
}

inline auto WindowRenderer::sdlCanvasTexture() -> SDL_Texture*
{
    return m_CanvasTexture;
}
