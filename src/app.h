#pragma once

#include "common.h"

class App
{
public:
    App(std::string const& title, std::size_t width, std::size_t height);
    ~App();

    bool running() const;
    bool nextFrame() const;

    void handleInput();
    void tick();
    void render();

protected:
    // TODO: LogicSim should be running on it's own thread so it can freely from the UI or the main application tick
    std::unique_ptr<LogicSim> m_LogicSim;
    std::thread               m_LogicSimThread;

    std::unique_ptr<Renderer> m_Renderer;

    std::atomic<bool> m_CloseRequested = false;
};
