/*************************************************************************
 * File     : hash.h
 * Author   : libingli
 * Created  : Fri 15 Jan 2016 07:37:31 AM CST
*************************************************************************/
#ifndef _HASH_H_
#define _HASH_H_

#include <sys/types.h>

/* @brief BKDR Hash Function
 * @detail 本算法由于在Brian Kernighan与Dennis Ritchie的《The C Programming Language》
 * 一书被展示而得名，是一种简单快捷的hash算法，
 * 也是Java目前采用的字符串的Hash算法（累乘因子为31）
 */ 
inline 
size_t hash_bkdr(const void *key, unsigned int ksize, unsigned int range)
{  
    size_t hash = 0;
    const char *cursor;

    cursor = (const char *)key;
    while (ksize-- && NULL != cursor) 
    {         
        hash = hash * 131 + *cursor++;   
        // 也可以乘以31、131、1313、13131、131313..  
        // 有人说将乘法分解为位运算及加减法可以提高效率，
        // 如将上式表达为：hash = hash << 7 + hash << 1 + hash + ch;  
        // 但其实在Intel平台上，CPU内部对二者的处理效率都是差不多的，  
        // 我分别进行了100亿次的上述两种运算，发现二者时间差距基本为0
        // （如果是Debug版，分解成位运算后的耗时还要高1/3）；  
        // 在ARM这类RISC系统上没有测试过，
        // 由于ARM内部使用Booth's Algorithm来模拟32位整数乘法运算，
        // 它的效率与乘数有关：  
        // 当乘数8-31位都为1或0时，需要1个时钟周期  
        // 当乘数16-31位都为1或0时，需要2个时钟周期  
        // 当乘数24-31位都为1或0时，需要3个时钟周期  
        // 否则，需要4个时钟周期  
        // 因此，虽然我没有实际测试，但是我依然认为二者效率上差别不大
    }
    return (hash % range);  
} 

#endif /* _HASH_H_ */
