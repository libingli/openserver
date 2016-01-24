/*************************************************************************
 * File     : hash.h
 * Author   : libingli
 * Created  : Fri 15 Jan 2016 07:37:31 AM CST
*************************************************************************/
#ifndef _HASH_H_
#define _HASH_H_

#include <sys/types.h>

size_t hash_bkdr(const char *str, int length, unsigned int range);

#endif /* _HASH_H_ */
