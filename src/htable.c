/*************************************************************************
 * File     : htable.c
 * Author   : libingli
 * Created  : Tue 26 Jan 2016 07:13:29 PM CST
*************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "htable.h"
#include "hash.h"
#include "log.h"
#include "server.h"

static hnode_t* htable_create_node(void *key, unsigned int ksize)
{
    hnode_t *node;

    node = (hnode_t *)malloc(sizeof(hnode_t));
    if (NULL != node)
    {
        memset(node, 0, sizeof(hnode_t));
        node->key = key;
        node->value = key;  /* TODO */
        node->ksize = ksize;
        list_init(&node->list);
    }
    return node;
}

static void htable_destroy_node(hnode_t *node)
{
    if (NULL != node)
    {
        /* maybe need a callback function to free the key and value */
        free(node);
    }
    return;
}

static hbucket_t* htable_create_bucket(unsigned int size)
{
    hbucket_t *bkt = NULL;
    unsigned int i;

    bkt = (hbucket_t *)malloc(sizeof(hbucket_t) * size);
    if (NULL == bkt)
    {
        return NULL;
    }
    memset(bkt, 0, sizeof(hbucket_t) * size);
    for (i = 0; i < size; i++)
    {
        list_init(&(bkt[i].head));
    }

    return bkt;
}

static void htable_destroy_bucket(hbucket_t *bkt)
{
    if (NULL != bkt)
    {
        free(bkt);
    }
    return;
}

static hnode_t* 
htable_lookup_bucket(hbucket_t *bkt, void *key, unsigned int ksize)
{
    hnode_t *node = NULL;
    hnode_t *cursor = NULL;
    int cmp = 1;

    list_for_each_entry(&(bkt->head), cursor, list)
    {
        if (cursor->ksize == ksize)
        {
            cmp = memcmp(cursor->key, key, ksize);
        }
        if (0 == cmp)
        {
            node = cursor;
            break;
        }
    }

    return node;
}

static int htable_insert_bucket(hbucket_t *bkt, hnode_t *node)
{
    int err = -1;

    if (NULL == htable_lookup_bucket(bkt, node->key, node->ksize))
    {
        list_add_tail(&(bkt->head), &node->list);
        bkt->nelems ++; 
        err = 0;
    }
    return err;
}

static int htable_rehash_bucket(htable_t *ht, hbucket_t *bkt)
{
    hnode_t *cursor;
    hnode_t *next;
    unsigned int rehash;
    int err = 0;

    list_for_each_entry_safe(&bkt->head, cursor, next, list)
    {
        bkt->nelems --;
        list_remove(&cursor->list);
        rehash = hash_bkdr((char *)cursor->key, cursor->ksize, ht->resize); 
        err |= htable_insert_bucket(&(ht->rehash[rehash]), cursor);
    }
    return err;
}

static hnode_t* 
htable_remove_from_bucket(hbucket_t *bkt, void *key, unsigned int ksize)
{
    hnode_t *node;
    node = htable_lookup_bucket(bkt, key, ksize);
    if (NULL != node)
    {
        bkt->nelems --;
        list_remove(&(node->list));
    }
    return node;
}

/*
 * Shrink table beneath 25% load
 * returns true if nelems < 0.25 * bkt->size
 */
static int htable_shrink_below_watermark(htable_t *ht)
{
    return ((ht->nelems < (ht->size / 4)) &&
            (ht->size > ht->min_size));
}

/*
 * Expand table when exceeding 75% load 
 * returns true if nelems > 0.75 * bkt->size
 */
static int htable_grow_above_watermark(htable_t *ht)
{
    return ((ht->nelems > (ht->size * 3 / 4)) &&
            (ht->max_size == 0 || ht->size < ht->max_size));
}

static int htable_rehash(htable_t *ht)
{
    int err = 0;
    unsigned int i;
    hbucket_t *bkt;

    for (i = 0; i < ht->size; i++)
    {
        err |= htable_rehash_bucket(ht, &(ht->hash[i]));
    }
    if (0 == err)
    {
        bkt = ht->hash;

        ht->size = ht->resize;
        ht->hash = ht->rehash;
        ht->resize = 0;
        ht->rehash = NULL;
        htable_destroy_bucket(bkt);
    }
    return 0;
}

static int htable_shrink(htable_t *ht)
{
    hbucket_t *rehash = NULL;
    int err = 0;

    rehash = htable_create_bucket(ht->size / 2);
    if (NULL != rehash)
    {
        ht->rehash = rehash;
        ht->resize = ht->size / 2;
        log_info("Hash resize: %d", ht->resize);
        err |= htable_rehash(ht);
    }
    return err;
}

static int htable_expand(htable_t *ht)
{
    hbucket_t *rehash = NULL;
    int err = 0;

    rehash = htable_create_bucket(ht->size * 2);
    if (NULL != rehash)
    {
        ht->rehash = rehash;
        ht->resize = ht->size * 2;
        log_info("Hash resize: %d", ht->resize);
        err |= htable_rehash(ht);
    }

    return err;
}

htable_t* htable_create(unsigned int min_size, unsigned int max_size)
{
    htable_t *ht = NULL;
    
    ht = (htable_t *)malloc(sizeof(htable_t));
    if (NULL != ht)
    {
        memset(ht, 0, sizeof(htable_t));
        ht->min_size = min_size;
        ht->max_size = max_size;
        ht->elasticity = 16;
        
        ht->size = min_size;
        ht->hash = htable_create_bucket(min_size);
    }

    return ht;
}

void htable_destroy(htable_t *ht)
{
    return;
}

hnode_t* htable_lookup(htable_t *ht, void *key, unsigned int ksize)
{
    unsigned int hash;
    hnode_t *node;

    hash = hash_bkdr(key, ksize, ht->size);
    node = htable_lookup_bucket(&(ht->hash[hash]), key, ksize);
    if (NULL == node && NULL != ht->rehash)
    {
        hash = hash_bkdr(key, ksize, ht->resize);
        node = htable_lookup_bucket(&(ht->rehash[hash]), key, ksize);
    }
    return node;
}

int htable_insert(htable_t *ht, void *key, unsigned int ksize)
{
    int err = 0;
    unsigned int hash;
    hnode_t *node;

    node = htable_create_node(key, ksize);
    if (NULL == node)
    {
        return -1;
    }
    if (NULL != ht->rehash)
    {
        hash = hash_bkdr(key, ksize, ht->resize);
        err = htable_insert_bucket(&(ht->rehash[hash]), node);
        if (0 == err)
        {
            ht->nelems ++;
        }
        /* else case is alread exist */
        return err;
    }

    hash = hash_bkdr((char *)key, ksize, ht->size);
    err = htable_insert_bucket(&(ht->hash[hash]), node);
    if (0 == err)
    {
        ht->nelems ++;
    }
    if (htable_grow_above_watermark(ht))
    {
        htable_expand(ht);
    }

    return err;
}

/* the return type is not reasonable */
void* htable_remove(htable_t *ht, void *key, unsigned int ksize)
{
    unsigned int hash;
    hnode_t *node;
    void *value = NULL;

    hash = hash_bkdr(key, ksize, ht->size);
    node = htable_remove_from_bucket(&(ht->hash[hash]), key, ksize);
    if (NULL == node && NULL != ht->rehash)
    {
        hash = hash_bkdr((char *)key, ksize, ht->resize);
        node = htable_remove_from_bucket(&(ht->rehash[hash]), key, ksize);
    }

    if (NULL != node)
    {
        ht->nelems --;
        value = node->value;
        htable_destroy_node(node);
        node = NULL;
    }

    if (htable_shrink_below_watermark(ht))
    {
        htable_shrink(ht);
    }
    return value;
}

void htable_state(htable_t *ht)
{
    unsigned int i;
    unsigned int collision = 0;

    log_info("Hash: %u", ht->size);
    for (i = 0; i < ht->size; i++)
    {
        collision += max(1, ht->hash[i].nelems) - 1;
        log_info("bucket[%03d] : %d", i, ht->hash[i].nelems);
    }
    
    log_info("REHash: %u", ht->resize);
    for (i = 0; i < ht->resize; i++)
    {
        collision += max(1, ht->rehash[i].nelems) - 1;
        log_info("bucket[%03d] : %d", i, ht->rehash[i].nelems);
    }
    log_info("Elements : %u", ht->nelems);
    log_info("Collision: %u", collision);
    return;
}
