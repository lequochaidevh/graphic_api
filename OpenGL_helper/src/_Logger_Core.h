#pragma once

#include "_Logger_encrypt_api.h"
#include "_Logger.h"

#define CORE_PRINT_TERMINAL

void Logger::setup_corelog(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (core_file_.is_open()) {
        core_file_.close();
        core_file_.clear();  // reset failbit
    }
    core_file_.open(filename, std::ios::out | std::ios::app | std::ios::binary);
}

template <typename... Args>
void Logger::core_log(LogLevel msg_level, const char* file, int line,
                      const char* func, const char* fmt_str, Args&&... args) {
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

    std::string final_msg = fmt::format("[{}][P:{}][T:{}] {}:{} {} : {}", time,
                                        pid, tid, file, line, func, msg);

    std::string log_msg_with_color =
        get_color_code(msg_level) + level_to_string(msg_level) +
        std::move(final_msg) + reset_color_code() + '\n';
    log_msg_with_color = SecretAPI::BasicEncrypt(log_msg_with_color);

    if (backend_ && backend_->get_is_running_status()) {
        backend_->log(std::move(log_msg_with_color));
        return;
    } else {
        // fallback sync
        std::lock_guard<std::mutex> lock(mutex_);

        if (core_file_.is_open()) {
            core_file_ << log_msg_with_color << '\n';
            core_file_.flush();
        }
    }

#ifdef CORE_PRINT_TERMINAL
    print_colored(msg_level, final_msg);
#endif
}

#define CORE_LOG_BACKEND             AsyncLogger
#define ASYNC_CORE_LOGGER_FILE_PATH  "/tmp/async_core.log"
#define CORE_LOGER_FILE_DEFAULT_PATH "/tmp/sync_core.log"
#define CORE_LOG_SET_FILE_STORE(X)   Logger::instance().setup_corelog(X)

#define CORE_LOG_SET_BACKEND \
    Logger::instance().set_backend(CORE_LOG_BACKEND::instance())
#define CORE_LOG_BACKEND_START \
    CORE_LOG_BACKEND::instance().start(ASYNC_CORE_LOGGER_FILE_PATH)
#define CORE_LOG_BACKEND_STOP CORE_LOG_BACKEND::instance().stop()

#define CORE_LOG_IMPL(level, fmt, ...)                                    \
    do {                                                                  \
        if constexpr (is_enabled(level)) {                                \
            Logger::instance().core_log(level, short_file_name(__FILE__), \
                                        __LINE__, __func__, fmt,          \
                                        ##__VA_ARGS__);                   \
        }                                                                 \
    } while (0)

#define CORE_LOG_TRACE(fmt, ...) \
    CORE_LOG_IMPL(LogLevel::TRACE, fmt, ##__VA_ARGS__)
#define CORE_LOG_DEBUG(fmt, ...) \
    CORE_LOG_IMPL(LogLevel::DEBUG, fmt, ##__VA_ARGS__)
#define CORE_LOG_INFO(fmt, ...) \
    CORE_LOG_IMPL(LogLevel::INFO, fmt, ##__VA_ARGS__)
#define CORE_LOG_WARN(fmt, ...) \
    CORE_LOG_IMPL(LogLevel::WARN, fmt, ##__VA_ARGS__)
#define CORE_LOG_ERROR(fmt, ...) \
    CORE_LOG_IMPL(LogLevel::ERROR, fmt, ##__VA_ARGS__)
#define CORE_LOG_CRITICAL(fmt, ...) \
    CORE_LOG_IMPL(LogLevel::CRITICAL, fmt, ##__VA_ARGS__)

#define ASYNC_CORE_LOG_IMPL(LOGFUNC, ...)                          \
    if (Logger::instance().get_backend_is_asigned_status()) {      \
        if (!CORE_LOG_BACKEND::instance().get_is_running_status()) \
            CORE_LOG_BACKEND_START;                                \
        LOGFUNC(__VA_ARGS__);                                      \
    }
// clang-format off
#define async_core_log_trace(...) ASYNC_CORE_LOG_IMPL(CORE_LOG_TRACE, __VA_ARGS__)
#define async_core_log_debug(...) ASYNC_CORE_LOG_IMPL(CORE_LOG_DEBUG, __VA_ARGS__)
#define async_core_log_info(...)  ASYNC_CORE_LOG_IMPL(CORE_LOG_INFO, __VA_ARGS__)
#define async_core_log_warn(...)  ASYNC_CORE_LOG_IMPL(CORE_LOG_WARN, __VA_ARGS__)
#define async_core_log_error(...) ASYNC_CORE_LOG_IMPL(CORE_LOG_ERROR, __VA_ARGS__)
#define async_core_log_critical(...) ASYNC_CORE_LOG_IMPL(CORE_LOG_CRITICAL, __VA_ARGS__)
// clang-format on