#pragma once

#define SPDLOG_FMT_EXTERNAL
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/fmt/fmt.h>
#include <stdexcept>
#include <memory>
#include <iostream>

#include "engine/resource/asset/animation.h"


namespace Yurrgoht {

	enum class ELogLevel {
		Debug, Info, Warning, Error, Fatal, ConsoleInfo
	};

    class LogSystem {
	public:
		void init();
		void destroy();
		std::vector<std::string> getLastestLogs();

		template<typename... TARGS>
		void log(ELogLevel level, TARGS&&... args) {
			switch (level)
			{
			case ELogLevel::Debug:
				c_logger->info(std::forward<TARGS>(args)...);
				m_logger->debug(std::forward<TARGS>(args)...);
				break;
			case ELogLevel::Info:
				c_logger->info(std::forward<TARGS>(args)...);
				m_logger->info(std::forward<TARGS>(args)...);
				break;
			case ELogLevel::Warning:
				c_logger->info(std::forward<TARGS>(args)...);
				m_logger->warn(std::forward<TARGS>(args)...);
				break;
			case ELogLevel::Error:
				c_logger->info(std::forward<TARGS>(args)...);
				m_logger->error(std::forward<TARGS>(args)...);
				break;
			case ELogLevel::Fatal: {
				c_logger->info(std::forward<TARGS>(args)...);
				m_logger->critical(args...);

				// throw application runtime error
				std::string fatal_str = fmt::format(std::forward<TARGS>(args)...);
				throw std::runtime_error(fatal_str);
			}
				break;
			default:
				break;
			}
		}

		template<typename... TARGS>
		void console_log(ELogLevel level, TARGS&&... args) {
			switch (level)
			{
			case ELogLevel::Debug:
				c_logger->debug(std::forward<TARGS>(args)...);
				break;
			case ELogLevel::Info:
				c_logger->info(std::forward<TARGS>(args)...);
				//m_logger->flush();
				break;
			case ELogLevel::Warning:
				c_logger->warn(std::forward<TARGS>(args)...);
				break;
			case ELogLevel::Error:
				c_logger->error(std::forward<TARGS>(args)...);
				break;
			case ELogLevel::Fatal: {
				c_logger->critical(args...);

				// throw application runtime error
				std::string fatal_str = fmt::format(std::forward<TARGS>(args)...);
				throw std::runtime_error(fatal_str);
			}
				break;
			default:
				break;
			}
		}

	private:
		std::string getLogFilename();

		std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> m_ringbuffer_sink;
		std::shared_ptr<spdlog::logger> m_logger;
		std::shared_ptr<spdlog::logger> c_logger;
	};
}

template <>
struct fmt::formatter<Yurrgoht::AnimationChannel::EPathType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Yurrgoht::AnimationChannel::EPathType& type, FormatContext& ctx) const { // Add `const` here
        std::string name;
        switch (type) {
            case Yurrgoht::AnimationChannel::EPathType::Translation:
                name = "Translation";
                break;
            case Yurrgoht::AnimationChannel::EPathType::Rotation:
                name = "Rotation";
                break;
            case Yurrgoht::AnimationChannel::EPathType::Scale:
                name = "Scale";
                break;
            default:
                name = "Unknown";
        }
        return fmt::format_to(ctx.out(), "{}", name);
    }
};
