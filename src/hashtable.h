/*************************************************************************
 * File     : hashtable.h
 * Author   : libingli
 * Created  : Tue 26 Jan 2016 07:13:21 PM CST
*************************************************************************/
#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "list.h"

typedef struct hashnode
{
    unsigned int    ksize;
    void            *key;
    void            *value;
    list_t          list;
}hashnode_t;

typedef struct hashbucket
{
    list_t          head;
    unsigned int    nelems;
    //lock
}hashbucket_t;

typedef struct hashtable
{
    unsigned int    min_size;
    unsigned int    max_size;
    unsigned int    elasticity; /* The maximum chain length of the hash table */
    
    unsigned int    nelems;
    unsigned int    size;
    unsigned int    resize;
    hashbucket_t    *hash;
    hashbucket_t    *rehash;
}hashtable_t;

int hashtable_insert(hashtable_t *ht, void *key, unsigned int ksize);
hashnode_t* hashtable_lookup(hashtable_t *ht, void *key, unsigned int ksize);
void* hashtable_remove(hashtable_t *ht, void *key, unsigned int ksize);

void hashtable_state(hashtable_t *ht);

hashtable_t* hashtable_create(unsigned int min_size, unsigned int max_size);
void hashtable_destroy(hashtable_t *ht);

#endif /* _HASHTABLE_H_ */
