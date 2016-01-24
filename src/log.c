/*************************************************************************
    > File Name     : log.c
    > Author        : libingli
    > Created Time  : Mon 17 Aug 2015 10:12:14 PM CST
 ************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "server.h"
#include "stropt.h"
#include "log.h"

STATIC FILE *g_log_file = NULL;

/* log switch levlel, 
 * the log of (level > sw_levle) do not output. */
STATIC int g_log_level = LOG_DEBUG;

STATIC const char *g_log_levelstr[] = {
    "EMERG",
    "ALERT",
    "CRIT",
    "ERROR",
    "WARN",
    "NOTICE",
    "INFO",
    "DEBUG"
};

/* Format: yyyymmdd hh:mm:ss.xxx */
STATIC int log_get_time(char *buf, int buflen)
{
    struct tm tm;
    struct timeval tv;
    int len = 0;
   
    gettimeofday(&tv, NULL); 
    localtime_r(&tv.tv_sec, &tm); 
    len += strftime(buf + len, buflen - len, "%Y%m%d %H:%M:%S", &tm); 
    len += scnprintf(buf + len, buflen - len, ".%d ", (int)(tv.tv_usec)/1000); 

    return len;
}

/*
 * Caution: Append '\n' at the end of line
 */
void log_output(int level, char *format, ...)
{
    va_list argp;
    pthread_t pid;
    char buf[MAX_BUF_LEN];
    int bufsize = 0;
    int offset = 0;

    if (level > g_log_level)
    {
        return;
    } 
    bufsize = sizeof(buf);
    buf[0] = '\0';
    pid = pthread_self();
   
    va_start(argp, format);
    offset += scnprintf(buf + offset, bufsize - offset, "[%x] ", (unsigned long)pid);
    offset += log_get_time(buf + offset, bufsize - offset);
    offset += scnprintf(buf + offset, bufsize - offset, "[%s] ", g_log_levelstr[level]);
    offset += vscnprintf(buf + offset, bufsize - offset, format, argp);
    /* Append '\n' at the end of line */
    offset += scnprintf(buf + offset, bufsize - offset, "\n");
    va_end(argp);
    if (NULL != g_log_file)
    {
        fwrite(buf, offset, 1, g_log_file); 
        fflush(g_log_file);
    }
    else
    {
        printf("%s", buf);
    }
    
    return;
}

void log_init(void)
{
    return;
}

void log_finish(void)
{
    if (NULL != g_log_file)
    {
        fclose(g_log_file);
    }
    return;
}
