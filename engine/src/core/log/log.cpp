#include "core/log/log.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

namespace Zafkiel
{
std::shared_ptr<spdlog::logger> &Log::GetCoreLogger()
{
    static bool init = false;
    static std::shared_ptr<spdlog::logger> coreLogger;
    if (!init)
    {
        init = true;
        spdlog::set_pattern("%^[%T] %n: %v%$");
        coreLogger = spdlog::stdout_color_mt("ZAFKIEL");
        coreLogger->set_level(spdlog::level::trace);
    }
    return coreLogger;
}

std::shared_ptr<spdlog::logger> &Log::GetClientLogger()
{
    static bool init = false;
    static std::shared_ptr<spdlog::logger> clientLogger;
    if (!init)
    {
        init = true;
        spdlog::set_pattern("%^[%T] %n: %v%$");
        clientLogger = spdlog::stdout_color_mt("APP");
        clientLogger->set_level(spdlog::level::trace);
    }
    return clientLogger;
}

}