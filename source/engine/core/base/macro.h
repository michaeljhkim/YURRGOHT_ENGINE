#pragma once

#include "engine/core/log/log_system.h"
#include "engine/platform/file/file_system.h"
#include "engine/function/global/engine_context.h"

#ifndef DEBUG
#define DEBUG (!NDEBUG)
#endif

#define APP_NAME "YurrgohtEditor"
#define APP_MAJOR_VERSION 1
#define APP_MINOR_VERSION 0
#define APP_PATCH_VERSION 0


// Define macros based on the build type
#if DEBUG
    #define LOG_HELPER(LOG_LEVEL, ...) \
        g_engine.logSystem()->log(LOG_LEVEL, "[" + std::string(__PRETTY_FUNCTION__) + "] " + std::format(__VA_ARGS__) ) 

    #define LOG_DEBUG(...) LOG_HELPER(ELogLevel::Debug, __VA_ARGS__)
    #define LOG_INFO(...) LOG_HELPER(ELogLevel::Info, __VA_ARGS__)
    #define LOG_WARNING(...) LOG_HELPER(ELogLevel::Warning, __VA_ARGS__)
    #define LOG_ERROR(...) LOG_HELPER(ELogLevel::Error, __VA_ARGS__)
    #define LOG_FATAL(...) LOG_HELPER(ELogLevel::Fatal, __VA_ARGS__)
    
    #define ASSERT(val, ...) \
        if (!(val)) { \
            LOG_FATAL(__VA_ARGS__); \
        }

    #define TO_ABSOLUTE(path) g_engine.fileSystem()->absolute(path)

#else
    // In release build, define them as no-ops
    #define LOG_HELPER(LOG_LEVEL, ...) (void)0
    #define LOG_DEBUG(...) (void)0
    #define LOG_INFO(...) (void)0
    #define LOG_WARNING(...) (void)0
    #define LOG_ERROR(...) (void)0
    #define LOG_FATAL(...) (void)0
    
    #define ASSERT(val, ...) (void)0

    #define TO_ABSOLUTE(path) (path)
#endif
