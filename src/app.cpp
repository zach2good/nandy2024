#include "app.h"

#include "logic/logic_sim.h"
#include "ui/renderer.h"
#include "ui/ui.h"
#include "applog_sink.h"

#include <SDL.h>

#include <memory>
#include "spdlog/async.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

App::App(std::string const& title, std::size_t width, std::size_t height)
: m_Renderer(std::make_unique<Renderer>(title, width, height))
{
    // Set up spdlog to output to AppLog
    spdlog::init_thread_pool(8192, 1);
    spdlog::flush_on(spdlog::level::warn);
    spdlog::flush_every(std::chrono::seconds(1));
    std::vector<spdlog::sink_ptr> sinks;
    auto sink = std::make_shared<spdlog::sinks::applog_sink_mt>();
    sinks.emplace_back(sink);
    auto logger = std::make_shared<spdlog::async_logger>("default", sinks.begin(), sinks.end(), spdlog::thread_pool());
    spdlog::set_default_logger(logger);

    spdlog::info("App::App()");

    // TODO: LogicSim should encapsulate it's own thread - that's an implementation detail.
    m_LogicSim = std::make_unique<LogicSim>();
    m_LogicSimThread = std::thread([this]()
    {
        while (this->running())
        {
            if (m_LogicSim->running())
            {
                m_LogicSim->step();
            }
        }
    });
}

App::~App()
{
    m_LogicSimThread.join();
}

bool App::running() const
{
    return !m_CloseRequested;
}

bool App::nextFrame() const
{
    // TODO: Frame delta, 60fps etc.
    std::chrono::milliseconds frameTime(16);
    std::this_thread::sleep_for(frameTime);
    return true;
}

void App::handleInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        m_Renderer->handleInput(event);
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

void App::tick()
{
}

void App::render()
{
    m_Renderer->clear();
    m_Renderer->draw(*m_LogicSim);
    m_Renderer->present();
}
