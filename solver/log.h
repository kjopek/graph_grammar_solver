#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdint.h>

enum log_level {
	LOG_DEBUG_LVL = 0,
	LOG_INFO_LVL = 1,
	LOG_WARN_LVL = 2,
	LOG_ERROR_LVL = 3,
	LOG_FATAL_LVL = 4,
};


#define LOG_MSG(level, msg, ...) \
	log_msg(level, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define LOG_DEBUG(msg, ...) \
	LOG_MSG(LOG_DEBUG_LVL, msg, ##__VA_ARGS__)

#define LOG_INFO(msg, ...) \
	LOG_MSG(LOG_INFO_LVL, msg, ##__VA_ARGS__)

#define LOG_WARNING(msg, ...) \
	LOG_MSG(LOG_WARN_LVL, msg, ##__VA_ARGS__)

#define LOG_ERROR(msg, ...) \
	LOG_MSG(LOG_ERROR_LVL, msg, ##__VA_ARGS__)

#define LOG_FATAL(msg, ...) \
	LOG_MSG(LOG_FATAL_LVL, msg, ##__VA_ARGS__)

#define LOG_ASSERT(cond, fmt, ...) ((cond) ? (void)0 : __assertx(__func__, \
	 __FILE__, __LINE__, #cond, fmt, ##__VA_ARGS__))

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void log_msg(enum log_level level, const char *filename, int line, const char *msg, ...);
void __assertx(const char *func, const char *file, int line, const char *e,
    const char *fmt, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOG_H */
