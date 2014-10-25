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
#ifndef LOG_H
#define LOG_H

#define DEBUG 1

#define LOGFILE PACKAGE_NAME ".log"

#define STRLEN 256
#define TIMEFORMAT  "%x %X"

#if DEBUG
#define FUNCTION_START Log(LOG_DEBUG, "%s starting\n", __func__);
#define FUNCTION_END Log(LOG_DEBUG, "%s finishing\n", __func__);
#else
#define FUNCTION_START
#define FUNCTION_END
#endif


#ifdef PTHREAD_HAVE
#define LOCK(mutex) do { pthread_mutex_t * _yymutex = &(mutex); \
	assert(pthread_mutex_lock(_yymutex)==0);
#define END_LOCK assert(pthread_mutex_unlock(_yymutex)==0); } while(0);
#else
#define LOCK
#define END_LOCK
#endif
#define BUF_MIN_LEN STRLEN

#define EOK 0
#define E_ERROR -1


typedef enum
{
	LOG_CRITICAL,
	LOG_ERROR,
	LOG_WARNING,
	LOG_ALERT,
	LOG_INFO,
	LOG_DEBUG
} LogType;

/**
 * Opens the log file to be written to
 */
int log_init();

/**
 * Closes the log file
 */
void log_close();

/**
 * Logs the given string to the log file
 * @param priority the priority of the message
 * @param s A formated (printf-style) string to log
 */
void Log(int priority, const char *s, ...);

#endif /* LOG_H */
