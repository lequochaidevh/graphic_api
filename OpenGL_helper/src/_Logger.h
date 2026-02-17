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

#include <queue>  // async log
#include <condition_variable>
#include <atomic>
#include <memory>

static inline uint32_t get_tid() {
    return static_cast<uint32_t>(::syscall(SYS_gettid));
}

inline const char* short_file_name(const char* path) {
    const char* file = std::strrchr(path, '/');
    return file ? file + 1 : path;
}

enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF };

#ifndef LOG_ACTIVE_LEVEL  // compile time
#define LOG_ACTIVE_LEVEL LogLevel::TRACE
#endif

constexpr bool is_enabled(LogLevel level) {
    return static_cast<int>(level) >= static_cast<int>(LOG_ACTIVE_LEVEL);
}

class AsyncLogger {
 public:
    static AsyncLogger& instance() {
        static AsyncLogger inst;
        return inst;
    }

    void start(const std::string& filename) {
        running_ = true;
        file_.open(filename, std::ios::out | std::ios::app);

        worker_ = std::thread([this] { this->process(); });
    }

    void stop() {
        running_ = false;
        cv_.notify_all();
        if (worker_.joinable()) worker_.join();

        file_.flush();
        file_.close();
    }

    void log(std::string msg) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(msg));
        }
        cv_.notify_one();
    }

    bool get_is_running_status() const { return running_.load(); }

 private:
    AsyncLogger() = default;
    ~AsyncLogger() { stop(); }

    void process() {
        while (running_ || !queue_.empty()) {
            std::unique_lock<std::mutex> lock(mutex_);

            cv_.wait(lock, [&] { return !running_ || !queue_.empty(); });

            while (!queue_.empty()) {
                auto msg = std::move(queue_.front());
                queue_.pop();
                lock.unlock();

                file_ << msg << '\n';

                lock.lock();
            }
        }
    }

 private:
    std::queue<std::string> queue_;
    std::mutex              mutex_;
    std::condition_variable cv_;
    std::thread             worker_;
    std::atomic<bool>       running_{false};
    std::ofstream           file_;
};

class Logger {
 public:
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    void set_level(LogLevel lvl) { level_ = lvl; }

    void set_file(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_.is_open()) {
            file_.close();
            file_.clear();  // reset failbit
        }
        file_.open(filename, std::ios::out | std::ios::app | std::ios::binary);
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

        std::string log_msg_with_color =
            get_color_code(msg_level) + level_to_string(msg_level) +
            std::move(final_msg) + reset_color_code();
        if (backend_ && backend_->get_is_running_status()) {
            backend_->log(std::move(log_msg_with_color));
            return;
        } else {
            // fallback sync
            std::lock_guard<std::mutex> lock(mutex_);
            if (file_.is_open()) file_ << log_msg_with_color << '\n';
        }

        print_colored(msg_level, final_msg);
    }

    void setup_corelog(const std::string& filename);
    template <typename... Args>
    void core_log(LogLevel msg_level, const char* file, int line,
                  const char* func, const char* fmt_str, Args&&... args);

 public:
    void set_backend(AsyncLogger& backend) { backend_ = &backend; }
    bool get_backend_is_asigned_status() { return backend_ != nullptr; }

 private:
    Logger() : level_(LogLevel::TRACE) {
        pthread_atfork(  // designed fork for singleton
            []() {
                if (Logger::instance().backend_)
                    Logger::instance().backend_->stop();
            },
            []() {}, []() { Logger::instance().backend_ = nullptr; });
    }
    ~Logger() {
        if (backend_->get_is_running_status()) backend_->stop();
    }
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
                           "{}", msg);  // fix bug bkgrd not clean
                fmt::print("\033[0m");
                fmt::print("{}", "\n");
                break;
        }
    }

    std::string get_color_code(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE:
                return "\033[37m";
            case LogLevel::DEBUG:
                return "\033[36m";
            case LogLevel::INFO:
                return "\033[32m";
            case LogLevel::WARN:
                return "\033[33m";
            case LogLevel::ERROR:
                return "\033[31m";
            case LogLevel::CRITICAL:
                return "\033[41m";
            default:
                return "\033[0m";
        }
    }

    std::string reset_color_code() { return "\033[0m"; }

    std::string level_to_string(LogLevel lvl) {
        switch (lvl) {
            case LogLevel::TRACE:
                return "[TRACE] ";
            case LogLevel::DEBUG:
                return "[DEBUG] ";
            case LogLevel::INFO:
                return "[INFO] ";
            case LogLevel::WARN:
                return "[WARN] ";
            case LogLevel::ERROR:
                return "[ERROR] ";
            case LogLevel::CRITICAL:
                return "[CRITICAL] ";
        }
        return "";
    }

 private:
    LogLevel      level_;
    std::ofstream file_, core_file_;
    std::mutex    mutex_;

 private:
    AsyncLogger* backend_ = nullptr;
};

#define LOG_BACKEND AsyncLogger

#define ASYNC_LOGER_FILE_PATH   "/tmp/async.log"
#define LOGER_FILE_DEFAULT_PATH "/tmp/sync.log"
#define LOG_SET_FILE_STORE(X)   Logger::instance().set_file(X)

#define LOG_SET_BACKEND   Logger::instance().set_backend(LOG_BACKEND::instance())
#define LOG_BACKEND_START LOG_BACKEND::instance().start(ASYNC_LOGER_FILE_PATH)
#define LOG_BACKEND_STOP  LOG_BACKEND::instance().stop()

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

#define ASYNC_LOG_IMPL(LOGFUNC, ...)                          \
    if (Logger::instance().get_backend_is_asigned_status()) { \
        if (!LOG_BACKEND::instance().get_is_running_status()) \
            LOG_BACKEND_START;                                \
        LOGFUNC(__VA_ARGS__);                                 \
    }

#define async_log_trace(...)    ASYNC_LOG_IMPL(LOG_TRACE, __VA_ARGS__)
#define async_log_debug(...)    ASYNC_LOG_IMPL(LOG_DEBUG, __VA_ARGS__)
#define async_log_info(...)     ASYNC_LOG_IMPL(LOG_INFO, __VA_ARGS__)
#define async_log_warn(...)     ASYNC_LOG_IMPL(LOG_WARN, __VA_ARGS__)
#define async_log_error(...)    ASYNC_LOG_IMPL(LOG_ERROR, __VA_ARGS__)
#define async_log_critical(...) ASYNC_LOG_IMPL(LOG_CRITICAL, __VA_ARGS__)

// clang-format off
// TODO:
// ok: ver1: Compile-time disable DEBUG - Zero-cost if level disabled
// ok: ver1: Colorized output
// ok: ver1: Async logging - GOOD NOTE about singleton...
// Ring buffer realtime safe
// Lock-free logger

// ok: ver1: Add thread and process id info:
// [26-02-17 04:10:14][P:49371][T:49372] Application.cpp:51 operator() : THREAD
// Worker running [26-02-17 04:10:14][P:49371][T:49371] Application.cpp:67 main
// : === PARENT === [26-02-17 04:10:14][P:49371][T:49371] Application.cpp:68
// main : getpid 49371 CHILD_PID: 49373 [26-02-17 04:10:14][P:49373][T:49373]
// Application.cpp:63 main : === CHILD === [26-02-17 04:10:14][P:49373][T:49373]
// Application.cpp:64 main : getpid 49373
// clang-format on
