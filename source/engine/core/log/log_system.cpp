#include "engine/core/base/macro.h"
#include <sstream>
#include <iomanip>
#include <ctime>

#define MAX_LOG_FILE_NUM 25
#define MAX_ROTATE_FILE_NUM 5
#define MAX_ROTATE_FILE_SIZE 1048576 * 10
#define MAX_RINGBUFFER_SIZE 100

namespace Yurrgoht {

    void LogSystem::init() {
        std::cout << "LogSystem::init - INIT (Info displayed without LOG_INFO)" << std::endl;
        // Console sink for immediate terminal output (synchronous)
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[%^%l%$] %v");

        // File sink for persistent storage with log rotation (asynchronous)
        std::string log_filename = getLogFilename();
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_filename, MAX_ROTATE_FILE_SIZE, MAX_ROTATE_FILE_NUM);
        file_sink->set_level(spdlog::level::trace);
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

        // Ring-buffer sink for real-time log storage in memory (asynchronous)
        m_ringbuffer_sink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(MAX_RINGBUFFER_SIZE);
        m_ringbuffer_sink->set_level(spdlog::level::trace);
        m_ringbuffer_sink->set_pattern("[%l] %v");

        // Create multi-sink logger (console sink will be synchronous, others async)
        spdlog::init_thread_pool(8192, 1);

#ifdef DEBUG
    const spdlog::sinks_init_list sink_list = { file_sink, m_ringbuffer_sink };

    // synchronous logger to flush logs to terminal immediately - only for console sink
    c_logger = std::make_shared<spdlog::logger>("console_sink_logger", console_sink);
    c_logger->flush_on(spdlog::level::info);
    spdlog::register_logger(c_logger);
#else
    const spdlog::sinks_init_list sink_list = { file_sink, m_ringbuffer_sink };
#endif

        // Create an async logger for the file and ring-buffer sinks
        m_logger = std::make_shared<spdlog::async_logger>("multi_sink_logger",
            sink_list.begin(), sink_list.end(), spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);
        m_logger->set_level(spdlog::level::trace);
        m_logger->flush_on(spdlog::level::trace); // Flush logs to file

        // Register the logger with spdlog
        spdlog::register_logger(m_logger);
    }

    void LogSystem::destroy() {
        m_logger->flush();
        spdlog::drop_all();
    }

	std::vector<std::string> LogSystem::getLastestLogs() {
        return m_ringbuffer_sink->last_formatted();
	}

	std::string getCurrentDateTimeStr() {
		auto current_time = std::time(nullptr);
		auto local_time = *std::localtime(&current_time);
		std::ostringstream oss;
		oss << std::put_time(&local_time, "%Y-%m-%d-%H-%M-%S");
		return oss.str();
    }

    std::string LogSystem::getLogFilename() {
        std::string log_dir = g_engine.fileSystem()->getLogDir();
        std::vector<std::string> log_filenames = g_engine.fileSystem()->traverse(log_dir, false, EFileOrderType::Time);
        if (log_filenames.size() >= MAX_LOG_FILE_NUM) {
            size_t removed_num = log_filenames.size() - MAX_LOG_FILE_NUM / 2;
            for (size_t i = 0; i < removed_num; ++i) {
                g_engine.fileSystem()->removeFile(log_filenames[i]);
            }
        }

        std::string log_filename = g_engine.fileSystem()->combine(log_dir, "Yurrgoht_" + getCurrentDateTimeStr() + ".log");
        return log_filename;
	}

}