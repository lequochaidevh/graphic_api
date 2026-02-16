#pragma once

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <fmt/color.h>

#include <thread>    // get info
#include <unistd.h>  // get process id
#include <sys/syscall.h>
#include <unistd.h>

static inline uint32_t get_tid() {
    return static_cast<uint32_t>(::syscall(SYS_gettid));
}

inline const char* short_file_name(const char* path) {
    const char* file = std::strrchr(path, '/');
    return file ? file + 1 : path;
}

enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF };

#ifndef LOG_ACTIVE_LEVEL
#define LOG_ACTIVE_LEVEL LogLevel::TRACE
#endif

constexpr bool is_enabled(LogLevel level) {
    return static_cast<int>(level) >= static_cast<int>(LOG_ACTIVE_LEVEL);
}

class Logger {
 public:
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    void set_level(LogLevel lvl) { level_ = lvl; }

    void set_file(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        file_.open(filename, std::ios::app);
    }

    template <typename... Args>
    void log(LogLevel msg_level, const char* file, int line, const char* func,
             const char* fmt_str, Args&&... args) {
        if (msg_level < level_) return;

        auto        now = std::chrono::system_clock::now();
        std::time_t t   = std::chrono::system_clock::to_time_t(now);

        std::tm tm_buf;
        localtime_r(&t, &tm_buf);

        char time_buffer[32];
        std::strftime(time_buffer, sizeof(time_buffer), "%y-%m-%d %H:%M:%S",
                      &tm_buf);

        std::string time = time_buffer;

        std::string msg = fmt::format(fmt_str, std::forward<Args>(args)...);

        pid_t    pid = ::getpid();
        uint32_t tid = get_tid();

        std::string final_msg =
            fmt::format("[{}][P:{}][T:{}] {}:{} {} : {}", time, pid, tid, file,
                        line, func, msg);

        std::lock_guard<std::mutex> lock(mutex_);

        print_colored(msg_level, final_msg);

        if (file_.is_open()) file_ << final_msg;
    }

 private:
    Logger() : level_(LogLevel::TRACE) {}

    void print_colored(LogLevel level, const std::string& msg) {
        switch (level) {
            case LogLevel::TRACE:
                fmt::print(fmt::fg(fmt::color::gray), "{}\n", msg);
                fmt::print("\033[0m");
                break;
            case LogLevel::DEBUG:
                fmt::print(fmt::fg(fmt::color::cyan), "{}\n", msg);
                fmt::print("\033[0m");
                break;

            case LogLevel::INFO:
                fmt::print(fmt::fg(fmt::color::lawn_green), "{}\n", msg);
                fmt::print("\033[0m");
                break;

            case LogLevel::WARN:
                fmt::print(fmt::fg(fmt::color::yellow), "{}\n", msg);
                fmt::print("\033[0m");
                break;

            case LogLevel::ERROR:
                fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold,
                           "{}\n", msg);
                fmt::print("\033[0m");
                break;
            case LogLevel::CRITICAL:
                fmt::print(fmt::fg(fmt::color::white) |
                               fmt::bg(fmt::color::red) | fmt::emphasis::bold,
                           "{}", msg);
                fmt::print("\033[0m");
                fmt::print("{}", "\n");
                break;
        }
    }

    const char* level_to_string(LogLevel lvl) {
        switch (lvl) {
            case LogLevel::TRACE:
                return "TRACE";
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARN:
                return "WARN";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::CRITICAL:
                return "CRITICAL";
        }
        return "";
    }

 private:
    LogLevel      level_;
    std::ofstream file_;
    std::mutex    mutex_;
};

#define LOG_IMPL(level, fmt, ...)                                              \
    do {                                                                       \
        if constexpr (is_enabled(level)) {                                     \
            Logger::instance().log(level, short_file_name(__FILE__), __LINE__, \
                                   __func__, fmt, ##__VA_ARGS__);              \
        }                                                                      \
    } while (0)

#define LOG_TRACE(fmt, ...)    LOG_IMPL(LogLevel::TRACE, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)    LOG_IMPL(LogLevel::DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)     LOG_IMPL(LogLevel::INFO, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)     LOG_IMPL(LogLevel::WARN, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)    LOG_IMPL(LogLevel::ERROR, fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) LOG_IMPL(LogLevel::CRITICAL, fmt, ##__VA_ARGS__)

// TODO:
// ok: ver1: Compile-time disable DEBUG
// Zero-cost if level disabled
// ok: ver1: Colorized output
// Async logging
// Ring buffer realtime safe
// Lock-free logger

// ok: ver1: Add thread and process id info:
// clang-format off
// [26-02-17 04:10:14][P:49371][T:49372] Application.cpp:51 operator() : THREAD Worker running
// [26-02-17 04:10:14][P:49371][T:49371] Application.cpp:67 main : === PARENT ===
// [26-02-17 04:10:14][P:49371][T:49371] Application.cpp:68 main : getpid 49371 CHILD_PID: 49373
// [26-02-17 04:10:14][P:49373][T:49373] Application.cpp:63 main : === CHILD ===
// [26-02-17 04:10:14][P:49373][T:49373] Application.cpp:64 main : getpid 49373
// clang-format on
