#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "server.h"

/**
 * scnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @...: Arguments for the format string
 *
 * The return value is the number of characters written into @buf not including
 * the trailing '\0'. If @size is <= 0 the function returns 0. If the return is
 * greater than or equal to @size, the resulting string is truncated.
 */
int scnprintf(char * buf, size_t size, const char *fmt, ...)
{
    va_list args;
    unsigned int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);
    return (i >= size) ? (size - 1) : i;
}

/**
 * vscnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * The return value is the number of characters which have been written into
 * the @buf not including the trailing '\0'. If @size is <= 0 the function
 * returns 0.
 *
 * Call this function if you are already dealing with a va_list.
 * You probably want scnprintf instead.
 */
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    unsigned int i;

    i=vsnprintf(buf,size,fmt,args);
    return (i >= size) ? (size - 1) : i;
}

/*
 * 功能：搜索字符串右边起的第一个匹配字符
 */
char* rlocate(char *str, char c)  
{ 
    int i = strlen(str); 
    if (!(*str)) 
    {
        return 0; 
    }
    while (str[i-1])  
    {
        if (strchr(str + (i - 1), c))     
        {
            return (str + (i - 1));    
        }
        else  
        {
            i--;
        }
    }
    return 0; 
} 
 
/*
 * 功能：把字符串转换为全小写
 */
void strlower(OUT char *str)  
{ 
    while (*str && *str != '\0')  
    {
        *str = tolower(*str);
        str++; 
    }
    *str = '\0';
} 

/* 
 * 删除左边的空格 
 */
char* lstrip(OUT char *str)
{
    char *left = NULL;

    assert(NULL != str);
    for (left = str; *left != '\0' && isspace(*left); left++)
    {
        ;
    }
    return left;
}
 
/* 
 * 删除右边的空格 
 */
void rstrip(OUT char *str)
{
    char *right = NULL;
    assert(NULL != str);
       
    for (right = str + strlen(str) - 1;right >= str && isspace(*right); --right)
    {
        ;
    }
    *(++right) = '\0';
       
    return;
}

/* 
 * 删除两边的空格 
 */
char* strip(OUT char *str)
{
    char *left = NULL;
    assert(str != NULL);
       
    left = lstrip(str);
    rstrip(left);

    return left;
}
/* Copy substring(from beginning of str to position fs) to line
 * @str: src string to find
 * @fd: field-separator 
 * @line: copy destnation
 * @linelen: length of @line
 *
 * return: length copied to line
 * 
 * If fs is not found, copy all str.
 */
int strline(const char *str, IN char *fs, OUT char *line, IN int linelen)
{
    int len = 0;
    const char *end = NULL;

    if (NULL == str || NULL == fs || NULL == line)
    {
        return len;
    }
    end = strstr(str, fs);
    if (NULL == end)
    {
        end = str + strlen(str); 
    }
    
    len += scnprintf(line + len, min(linelen - len, end - str + 1), str);

    return len;
}
