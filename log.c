#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "brick.h"

enum log_level log_level = LOG_INFO;

static const char *level_names[] = {
	[LOG_DEBUG]      = "DEBUG",
	[LOG_INFO]       = "INFO",
	[LOG_WARN]       = "WARN",
	[LOG_ERROR]      = "ERROR",
	[LOG_FATAL]      = "FATAL",
};

void loggerf(enum log_level level, const char *fmt, ...) {
	va_list va;
	time_t now;
	char buf[BUFSIZE];

	if (level < log_level) {
		return;
	}

	time(&now);
	strftime(buf, BUFSIZE, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
	fprintf(stderr, "%s [%s] ", buf, level_names[level]);

	va_start(va, fmt);
	vsnprintf(buf, BUFSIZE, fmt, va);
	va_end(va);

	fprintf(stderr, "%s\n", buf);
}
