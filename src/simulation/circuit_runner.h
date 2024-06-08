#pragma once

#include "types.h"

#include "simulation/circuit.h"
#include "simulation/commands/command.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class CircuitRunner
{
public:
    CircuitRunner();
    ~CircuitRunner();

    void start();
    void stop();
    void step();

    void sendCommand(std::unique_ptr<Command> command);
    auto circuit() -> Circuit&;

private:
    void run();

    std::unique_ptr<Circuit>             circuit_;
    std::thread                          thread_;
    std::atomic<bool>                    running_ = false;
    std::queue<std::unique_ptr<Command>> commandQueue_;
    std::mutex                           mutex_;
};

inline CircuitRunner::CircuitRunner()
{
    start();
}

inline CircuitRunner::~CircuitRunner()
{
    stop();
}

inline void CircuitRunner::start()
{
    if (running_)
    {
        return;
    }

    running_ = true;
    thread_  = std::thread(&CircuitRunner::run, this);
}

inline void CircuitRunner::stop()
{
    if (!running_)
    {
        return;
    }

    running_ = false;
    if (thread_.joinable())
    {
        thread_.join();
    }
}

inline void CircuitRunner::step()
{
    std::unique_lock<std::mutex> lock(mutex_);
    {
        while (!commandQueue_.empty())
        {
            auto commandPtr = std::move(commandQueue_.front());
            commandQueue_.pop();
            commandPtr->execute(*circuit_);
        }

        circuit_->step();
    }
}

inline void CircuitRunner::sendCommand(std::unique_ptr<Command> command)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        commandQueue_.push(std::move(command));
    }
}

inline auto CircuitRunner::circuit() -> Circuit&
{
    std::unique_lock<std::mutex> lock(mutex_);
    return *circuit_;
}

inline void CircuitRunner::run()
{
    while (running_)
    {
        step();
    }
}
