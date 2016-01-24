/*************************************************************************
    > File Name     : server.h
    > Author        : libingli
    > Created Time  : Wed 19 Aug 2015 09:26:13 PM CST
 ************************************************************************/

#ifndef _SERVER_H_
#define _SERVER_H_

#define INVALID_FD        -1

#define MAX_BUF_LEN       1024

#define OK                0
#define ERROR             -1

#define IN                    
#define OUT                    

#ifdef _DEBUG_
#define STATIC
#else
#define STATIC static
#endif

#define min(x,y)  ({ typeof(x) _x=(x), _y=(y); (_x < _y)?_x:_y; })
#define max(x,y)  ({ typeof(x) _x=(x), _y=(y); (_x > _y)?_x:_y; })

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#endif /* _SERVER_H_ */

