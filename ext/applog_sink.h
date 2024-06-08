#pragma once

#include <spdlog/details/file_helper.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/sinks/base_sink.h>

#include <mutex>

#include "applog.h"

namespace spdlog
{
    namespace sinks
    {
        template <typename Mutex>
        class applog_sink : public base_sink<Mutex>
        {
        protected:
            void sink_it_(const details::log_msg& msg) override
            {
                memory_buf_t formatted;
                applog_sink<Mutex>::formatter_->format(msg, formatted);
                AppLog::get().Log("%s", fmt::to_string(formatted).c_str());
            }
            void flush_() override
            {
            }
        };

        using applog_sink_mt = applog_sink<details::null_mutex>;
        using applog_sink_st = applog_sink<details::null_mutex>;

    } // namespace sinks

    template <typename Factory = spdlog::synchronous_factory>
    inline std::shared_ptr<logger> applog_logger_mt(const std::string& logger_name)
    {
        return Factory::template create<sinks::applog_sink_mt>(logger_name);
    }

    template <typename Factory = spdlog::synchronous_factory>
    inline std::shared_ptr<logger> applog_logger_st(const std::string& logger_name)
    {
        return Factory::template create<sinks::applog_sink_st>(logger_name);
    }
} // namespace spdlog
