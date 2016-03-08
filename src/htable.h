/*************************************************************************
 * File     : hashtable.h
 * Author   : libingli
 * Created  : Tue 26 Jan 2016 07:13:21 PM CST
*************************************************************************/
#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "list.h"

typedef struct hash_node
{
    unsigned int    ksize;
    void            *key;
    void            *value;
    list_t          list;
}hnode_t;

typedef struct hash_bucket
{
    list_t          head;
    unsigned int    nelems;
    //lock
}hbucket_t;

typedef struct hash_table
{
    unsigned int    min_size;
    unsigned int    max_size;
    unsigned int    elasticity; /* The maximum chain length of the hash table */
    
    unsigned int    nelems;
    unsigned int    size;
    unsigned int    resize;
    hbucket_t       *hash;
    hbucket_t       *rehash;
}htable_t;

int htable_insert(htable_t *ht, void *key, unsigned int ksize);
hnode_t* htable_lookup(htable_t *ht, void *key, unsigned int ksize);
void* htable_remove(htable_t *ht, void *key, unsigned int ksize);

void htable_state(htable_t *ht);

htable_t* htable_create(unsigned int min_size, unsigned int max_size);
void htable_destroy(htable_t *ht);

#endif /* _HASHTABLE_H_ */
