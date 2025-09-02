#pragma once

#include <spdlog/spdlog.h>

namespace Zafkiel
{

class Log
{
  public:
    Log() = delete;

    [[gnu::constructor]]
    static void Init();

    template <typename... Args>
    static void CoreTrace(fmt::format_string<Args...> format, Args &&...args)
    {
        coreLogger->trace(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Trace(fmt::format_string<Args...> format, Args &&...args)
    {
        clientLogger->trace(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreDebug(fmt::format_string<Args...> format, Args &&...args)
    {
        coreLogger->debug(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Debug(fmt::format_string<Args...> format, Args &&...args)
    {
        clientLogger->debug(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreInfo(fmt::format_string<Args...> format, Args &&...args)
    {
        coreLogger->info(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Info(fmt::format_string<Args...> format, Args &&...args)
    {
        clientLogger->info(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreWarn(fmt::format_string<Args...> format, Args &&...args)
    {
        coreLogger->warn(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Warn(fmt::format_string<Args...> format, Args &&...args)
    {
        clientLogger->warn(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreError(fmt::format_string<Args...> format, Args &&...args)
    {
        coreLogger->error(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Error(fmt::format_string<Args...> format, Args &&...args)
    {
        clientLogger->error(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreCritical(fmt::format_string<Args...> format, Args &&...args)
    {
        coreLogger->critical(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Critical(fmt::format_string<Args...> format, Args &&...args)
    {
        clientLogger->critical(format, std::forward<Args>(args)...);
    }
  private:
    static std::shared_ptr<spdlog::logger> coreLogger;
    static std::shared_ptr<spdlog::logger> clientLogger;
};

}