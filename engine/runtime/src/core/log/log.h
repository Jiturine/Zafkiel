#pragma once

#include <spdlog/spdlog.h>

namespace Zafkiel
{

class Log
{
  public:
    Log() = delete;

    template <typename... Args>
    static void CoreTrace(fmt::format_string<Args...> format, Args &&...args)
    {
        GetCoreLogger()->trace(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Trace(fmt::format_string<Args...> format, Args &&...args)
    {
        GetClientLogger()->trace(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreDebug(fmt::format_string<Args...> format, Args &&...args)
    {
        GetCoreLogger()->debug(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Debug(fmt::format_string<Args...> format, Args &&...args)
    {
        GetClientLogger()->debug(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreInfo(fmt::format_string<Args...> format, Args &&...args)
    {
        GetCoreLogger()->info(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Info(fmt::format_string<Args...> format, Args &&...args)
    {
        GetClientLogger()->info(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreWarn(fmt::format_string<Args...> format, Args &&...args)
    {
        GetCoreLogger()->warn(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Warn(fmt::format_string<Args...> format, Args &&...args)
    {
        GetClientLogger()->warn(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreError(fmt::format_string<Args...> format, Args &&...args)
    {
        GetCoreLogger()->error(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Error(fmt::format_string<Args...> format, Args &&...args)
    {
        GetClientLogger()->error(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void CoreCritical(fmt::format_string<Args...> format, Args &&...args)
    {
        GetCoreLogger()->critical(format, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Critical(fmt::format_string<Args...> format, Args &&...args)
    {
        GetClientLogger()->critical(format, std::forward<Args>(args)...);
    }
  private:
    static std::shared_ptr<spdlog::logger> &GetCoreLogger();
    static std::shared_ptr<spdlog::logger> &GetClientLogger();
};

}