/**
 * Log.c
 *
 * Author: Jonathan Chapple
 * Contact: joncpl@gmail.com
 * Date: April 12, 2014
 *
 * Provides log-to-file functionality
 *
 */
#include "config.h"
#include "Log.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef PTHREAD_HAVE
#include <pthread.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <assert.h>


static struct mylogpriority
{
	LogType priority;
	char *description;
} logPriority[] =
{
		{LOG_CRITICAL, "critical"},
		{LOG_ERROR, "error"},
		{LOG_WARNING, "warning"},
		{LOG_ALERT, "alert"},
		{LOG_INFO, "info"},
		{LOG_DEBUG, "debug"},
		{-1, NULL}
};

static FILE *log = NULL;

#ifdef PTHREAD_HAVE
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

int log_init()
{
	log = fopen(LOGFILE, "a+");
	if (!log)
	{
		return (0);
	}

	setvbuf(log, NULL, _IONBF, 0);
	return (1);
}

void log_close()
{
	if (log)
		fclose(log);
	log = NULL;
}

static char *timefmt()
{
	time_t now;
	struct tm tm;
	char buf[STRLEN];

	time(&now);
	tm = *localtime(&now);
	if (!strftime(buf, STRLEN, TIMEFORMAT, &tm))
	{
		buf[0] = 0;
	}
	return strdup(buf);
}

static const char *logPriorityDescp(int p) {
	struct mylogpriority *lp = logPriority;

	while ((*lp).description)
	{
		if (p == (*lp).priority)
		{
			return (*lp).description;
		}
		lp++;
	}
	return "unknown";
}

void Log(int priority, const char *s, ...)
{
	va_list ap;
	char *datetime = NULL;
	if (!log)
	{
		return;
	}


#if !DEBUG

	if (priority == LOG_DEBUG)
	{
		return;
	}
#endif

	va_start(ap, s);
	LOCK(log_mutex);

	datetime = timefmt(datetime, STRLEN);
	fprintf(log, "[%s] %-8s : ", datetime,
			logPriorityDescp(priority));
	vfprintf(log, s, ap);
	fprintf(log, "\n");
	free(datetime);

	END_LOCK

	va_end(ap);
}
