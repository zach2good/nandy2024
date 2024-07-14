#include "application.h"

#include "simulation/circuit_runner.h"
#include "simulation/commands/add_component_command.h"
#include "simulation/commands/command.h"

#include "ui/actions/action.h"
#include "ui/actions/ui_close_requested_action.h"

#include "ui/canvas_controller.h"
#include "ui/canvas_view_model.h"
#include "ui/renderers/ui_renderer.h"
#include "ui/renderers/window_renderer.h"
#include "ui/ui_input_handler.h"

#include <SDL.h>

#include <memory>
#include <vector>

Application::Application(std::string const& title, usize width, usize height)
{
    m_CircuitRunner    = std::make_unique<CircuitRunner>();
    m_WindowRenderer   = std::make_unique<WindowRenderer>(title, Size{ static_cast<f64>(width), static_cast<f64>(height) });
    m_UIRenderer       = std::make_unique<UIRenderer>(&*m_WindowRenderer);
    m_UIInputHandler   = std::make_unique<UIInputHandler>();
    m_CanvasController = std::make_unique<CanvasController>();

    m_CanvasViewModel = CanvasViewModel::create(m_CircuitRunner->circuit());
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
    // Events from SDL
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        m_WindowRenderer->handleEvent(event);
        m_UIRenderer->handleEvent(event);
        switch (event.type)
        {
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
    m_UIRenderer->clear();
    m_WindowRenderer->clear();

    m_WindowRenderer->setDrawToCanvas();
    m_WindowRenderer->clear();
    m_CanvasViewModel->draw(&*m_WindowRenderer);

    m_WindowRenderer->setDrawToScreen();
    auto actions = m_UIRenderer->draw();
    m_WindowRenderer->draw();

    for (auto& action : actions)
    {
        if (auto closeAction = dynamic_cast<UICloseRequestedAction*>(&*action))
        {
            m_CloseRequested = true;
        }
    }

    auto commands = std::vector<std::unique_ptr<Command>>();
    for (auto& event : m_UIInputHandler->handleInput(&*m_CanvasViewModel, std::move(actions)))
    {
        for (auto& command : m_CanvasController->handleCanvasEvent(std::move(event)))
        {
            commands.push_back(std::move(command));
        }
    }

    m_CircuitRunner->sendCommands(std::move(commands));

    m_CanvasViewModel->update();

    m_UIRenderer->present();
    m_WindowRenderer->present();
}
