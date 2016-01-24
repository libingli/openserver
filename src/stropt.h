#ifndef _STROPT_H_
#define _STROPT_H_

#include <stdarg.h> /* for va_list */

int scnprintf(char * buf, size_t size, const char *fmt, ...);
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);

char* lstrip(char *str);
char* strip(char *string);

int strline(const char *str, IN char *fs, OUT char *line, IN int linelen);

#endif /* _STROPT_H_ */
