#pragma once

#include "imports.h"


#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define TRACE_FILE_LOG "[EMBER_ENGINE - " __FILE__ ":" TO_STRING(__LINE__) "] "

/* WARNING: SHOULD BE ONLY USED BY THE ENGINE, IN DEVELOPMENT USE `Debug`  */
#define LOG_ERROR(...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, TRACE_FILE_LOG __VA_ARGS__);
#define LOG_INFO(...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, TRACE_FILE_LOG __VA_ARGS__);
#define LOG_DEBUG(...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, TRACE_FILE_LOG __VA_ARGS__);
#define LOG_VERBOSE(...) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, TRACE_FILE_LOG __VA_ARGS__);
#define LOG_WARN(...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, TRACE_FILE_LOG __VA_ARGS__);
#define LOG_TRACE(...) SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, TRACE_FILE_LOG __VA_ARGS__);
#define LOG_CRITICAL(...) SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, TRACE_FILE_LOG __VA_ARGS__);

#define LOG_QUIT_ON_FAIL(x) if(!x) { LOG_ERROR("%s", SDL_GetError()); return SDL_APP_FAILURE; }


// TODO: check for enum errors
#define GL_ERROR()                                  \
    {                                               \
        unsigned int error = glGetError();          \
        if (error != GL_NO_ERROR) {                 \
            LOG_ERROR("API ERROR_CODE: %d", error); \
        }                                           \
    }
