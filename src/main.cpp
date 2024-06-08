#include "application.h"
#include "config.h"
#include "types.h"

#include "applog_sink.h"
#include "spdlog/async.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void initLogging()
{
    // Set up spdlog to output to AppLog
    spdlog::init_thread_pool(8192, 1);
    spdlog::flush_on(spdlog::level::warn);
    spdlog::flush_every(std::chrono::seconds(1));

    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(std::make_shared<spdlog::sinks::applog_sink_mt>());
    sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    auto logger = std::make_shared<spdlog::async_logger>("default", sinks.begin(), sinks.end(), spdlog::thread_pool());
    spdlog::set_default_logger(logger);
}

int main(int argc, char** argv)
{
    initLogging();

    Application application(Config::kScreenTitle, Config::kScreenWidth, Config::kScreenHeight);

    while (application.running())
    {
        if (application.nextFrame())
        {
            application.handleInput();
            application.tick();
            application.render();
        }
    }

    return 0;
}
