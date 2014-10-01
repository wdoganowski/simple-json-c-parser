/*
 * MIT License http://dogan.mit-license.org/
 */

 #pragma once

// Use #define DEBUG_ON to enable debugs
// Use #define DEBUG_DEEP to enable deep debugs (on almost each call)

#ifdef DEBUG_DEEP

#ifndef DEBUG_ON
#define DEBUG_ON
#define DEEP_DEBUG_CODE(code) code
#endif

#else 
#define DEEP_DEBUG_CODE(code)
#endif

#ifdef DEBUG_ON

#ifdef PBL_APP_INFO
#define DEBUG_CODE(code) code
#define LOG_DEBUG(message, ...) APP_LOG(APP_LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) APP_LOG(APP_LOG_LEVEL_ERROR, message, ##__VA_ARGS__)
#else
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define DEBUG_CODE(code) code
#define LOG_DEBUG(fmt, ...) fprintf(stdout, "D %s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "%s\aE %s:%d:%s(): %s" fmt "\n", ANSI_COLOR_MAGENTA, __FILE__, __LINE__, __func__, __VA_ARGS__, ANSI_COLOR_RESET)
#endif

#else

#define DEBUG_CODE(code) do {} while (0)
#define LOG_DEBUG(message, ...) do {} while (0)
#define LOG_DEBUG(message, ...) do {} while (0)

#endif