#pragma once

#include "types.h"

#include <atomic>
#include <memory>
#include <string>

class CircuitRunner;
class WindowRenderer;
class UIRenderer;
class UIInputHandler;
class CanvasController;
class CanvasViewModel;

class Application final
{
public:
    Application(std::string const& title, usize width, usize height);
    ~Application();

    bool running() const;
    bool nextFrame() const;

    void handleInput();
    void tick();
    void render();

protected:
    std::atomic_bool m_CloseRequested = false;

    std::unique_ptr<CircuitRunner> m_CircuitRunner;

    std::unique_ptr<WindowRenderer>   m_WindowRenderer;
    std::unique_ptr<UIRenderer>       m_UIRenderer;
    std::unique_ptr<UIInputHandler>   m_UIInputHandler;
    std::unique_ptr<CanvasController> m_CanvasController;
    std::unique_ptr<CanvasViewModel>  m_CanvasViewModel;
};
