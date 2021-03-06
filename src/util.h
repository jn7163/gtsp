/* Copyright (c) 2015 Alinson Xavier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PROJECT_UTIL_H_
#define _PROJECT_UTIL_H_

#include <string.h>
#include "params.h"

#define LOG_LEVEL_ERROR 10
#define LOG_LEVEL_WARNING 20
#define LOG_LEVEL_INFO 30
#define LOG_LEVEL_DEBUG 40
#define LOG_LEVEL_VERBOSE 50

#if LOG_LEVEL < LOG_LEVEL_VERBOSE
#define log_verbose(...)
#else
#define log_verbose(...) time_printf( __VA_ARGS__)
#endif

#if LOG_LEVEL < LOG_LEVEL_DEBUG
#define log_debug(...)
#else
#define log_debug(...) time_printf( __VA_ARGS__)
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
    fprintf(stderr, msg " (%s:%d)\n", __FILE__, __LINE__); \
    rval = 1; goto CLEANUP; }

#define abort_iff(cond, msg, ...) if(cond) { \
    fprintf(stderr, msg " (%s:%d)\n", __VA_ARGS__, __FILE__, __LINE__); \
    rval = 1; goto CLEANUP; }

#define UNUSED(x) (void)(x)

void time_printf(const char *fmt, ...);

double get_user_time(void);

double get_real_time();

void pause();

#endif
