#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/fmt/fmt.h>
#include <stdexcept>
#include <memory>

namespace Yurrgoht
{
	enum class ELogLevel
	{
		Debug, Info, Warning, Error, Fatal
	};

    class LogSystem
    {
        public:
            void init();
            void destroy();
			std::vector<std::string> getLastestLogs();

            template<typename... TARGS>
            void log(ELogLevel level, TARGS&&... args)
            {	
				// NOTE: I have NO idea of if this REALLY works or how this works. I just kept asking chatgpt until I got no red lines on build
				// to be fair, it is 2:37am, and I did not want to learn. I just want this to vaguely work

				// Format the arguments into a string
				const auto formatted_message = fmt::format("{}", std::forward<TARGS>(args)...);	

				switch (level)
				{
				case ELogLevel::Debug:
					m_logger->debug(formatted_message);
					break;
				case ELogLevel::Info:
					m_logger->info(formatted_message);
					break;
				case ELogLevel::Warning:
					m_logger->warn(formatted_message);
					break;
				case ELogLevel::Error:
					m_logger->error(formatted_message);
					break;
				case ELogLevel::Fatal:
					{
						m_logger->critical(formatted_message);

						// throw application runtime error
						throw std::runtime_error(formatted_message);
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
    };
}
