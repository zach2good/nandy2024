#include "application.h"

#include "simulation/circuit_runner.h"

#include "ui/canvas_controller.h"
#include "ui/canvas_view_model.h"
#include "ui/renderers/ui_renderer.h"
#include "ui/renderers/window_renderer.h"

#include <SDL.h>

Application::Application(std::string const& title, usize width, usize height)
{
    m_CircuitRunner  = std::make_unique<CircuitRunner>();
    m_WindowRenderer = std::make_unique<WindowRenderer>(title, Size{ static_cast<f64>(width), static_cast<f64>(height) });
    m_UIRenderer     = std::make_unique<UIRenderer>(&*m_WindowRenderer);
}

Application::~Application()
{
    spdlog::info("Application::~Application()");
}

bool Application::running() const
{
    return !m_CloseRequested;
}

bool Application::nextFrame() const
{
    // TODO: Frame delta, 60fps etc.
    std::chrono::milliseconds frameTime(16);
    std::this_thread::sleep_for(frameTime);
    return true;
}

void Application::handleInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        m_WindowRenderer->handleEvent(event);
        m_UIRenderer->handleEvent(event);
        switch (event.type)
        {
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    spdlog::info("SDLK_ESCAPE -> SDL_QUIT requested");
                    m_CloseRequested = true;
                }
            }
            break;
            case SDL_QUIT:
            {
                spdlog::info("SDL_QUIT requested");
                m_CloseRequested = true;
            }
            break;
            default:
                break;
        }
    }
}

void Application::tick()
{
}

void Application::render()
{
    m_WindowRenderer->clear();
    m_UIRenderer->clear();

    m_WindowRenderer->setDrawToCanvas();
    auto model = CanvasViewModel::create(m_CircuitRunner->circuit());
    model->draw(&*m_WindowRenderer);

    m_WindowRenderer->setDrawToScreen();
    m_UIRenderer->draw();
    m_WindowRenderer->draw();

    m_UIRenderer->present();
    m_WindowRenderer->present();
}
