#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

static const char *log_level_msgs[] = {
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL",
	NULL
};
static enum log_level current_level = LOG_INFO_LVL;

#ifdef __cplusplus
extern "C" {
#endif

void
log_set_level(enum log_level level)
{

	current_level = level;
}

void
log_msg(enum log_level level, const char *filename, int line, const char *msg,
    ...)
{
	va_list list;
	const char *msg_fmt = "%s [%d]: %s\n";
	char *log_content;

	log_content = NULL;

	if (level < current_level)
		return;

	va_start(list, msg);
	(void)vasprintf(&log_content, msg, list);
	va_end(list);

	(void)fprintf(stderr, msg_fmt, filename, line, log_content);

	free(log_content);
}

void
__assertx(const char *func, const char *file, int line, const char *e,
    const char *fmt, ...)
{
	va_list list;
	char *assert_msg;

	assert_msg = NULL;
	log_msg(LOG_FATAL_LVL, file, line,
	    "Assertion failed: %s at %s:%d [%s]", e, file, line, func);

	va_start(list, fmt);
	(void)vasprintf(&assert_msg, fmt, list);
	va_end(list);

	if (assert_msg != NULL)
		log_msg(LOG_FATAL_LVL, file, line, assert_msg);
	free(assert_msg);

	abort();
}

#ifdef __cplusplus
}
#endif
