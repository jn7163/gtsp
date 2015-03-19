#ifndef _PROJECT_UTIL_H_
#define _PROJECT_UTIL_H_

#define LOG_LEVEL_ERROR 10
#define LOG_LEVEL_WARNING 20
#define LOG_LEVEL_INFO 30
#define LOG_LEVEL_DEBUG 40
#define LOG_LEVEL_VERBOSE 50

#define LOG_LEVEL LOG_LEVEL_INFO

#if LOG_LEVEL < LOG_LEVEL_DEBUG
#define log_verbose(...)
#else
#define log_verbose(...) time_printf( __VA_ARGS__)
#endif

#if LOG_LEVEL < LOG_LEVEL_VERBOSE
#define log_verbose(...)
#else
#define log_verbose(...) time_printf( __VA_ARGS__)
#endif

#if LOG_LEVEL < LOG_LEVEL_INFO
#define log_info(...)
#else
#define log_info(...) time_printf( __VA_ARGS__)
#endif

#if LOG_LEVEL < LOG_LEVEL_WARNING
#define log_warn(...)
#else
#define log_warn(...) time_printf( __VA_ARGS__)
#endif

#if LOG_LEVEL < LOG_LEVEL_ERROR
#define log_error(...)
#else
#define log_error(...) time_printf( __VA_ARGS__)
#endif

#define abort_if(cond, msg) if(cond) { \
    fprintf(stderr, "%20s:%d " msg "\n", __FILE__, __LINE__); \
    rval = 1; goto CLEANUP; }

void time_printf(const char *fmt, ...);

double get_current_time(void);

double get_real_time();

void next_set(int sz, int *set);

#endif
