/*************************************************************************
 * File     : hashtable.c
 * Author   : libingli
 * Created  : Tue 26 Jan 2016 07:13:29 PM CST
*************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "hash.h"
#include "log.h"
#include "server.h"

static hashnode_t* hashtable_create_node(void *key, unsigned int ksize)
{
    hashnode_t *node;

    node = (hashnode_t *)malloc(sizeof(hashnode_t));
    if (NULL != node)
    {
        memset(node, 0, sizeof(hashnode_t));
        node->key = key;
        node->value = key;  /* TODO */
        node->ksize = ksize;
        list_init(&node->list);
    }
    return node;
}

static void hashtable_destroy_node(hashnode_t *node)
{
    if (NULL != node)
    {
        /* maybe need a callback function to free the key and value */
        free(node);
    }
    return;
}

static hashbucket_t* hashtable_create_bucket(unsigned int size)
{
    hashbucket_t *bkt = NULL;
    unsigned int i;

    bkt = (hashbucket_t *)malloc(sizeof(hashbucket_t) * size);
    if (NULL == bkt)
    {
        return NULL;
    }
    memset(bkt, 0, sizeof(hashbucket_t) * size);
    for (i = 0; i < size; i++)
    {
        list_init(&(bkt[i].head));
    }

    return bkt;
}

static void hashtable_destroy_bucket(hashbucket_t *bkt)
{
    if (NULL != bkt)
    {
        free(bkt);
    }
    return;
}

static hashnode_t* 
hashtable_lookup_bucket(hashbucket_t *bkt, void *key, unsigned int ksize)
{
    hashnode_t *node = NULL;
    hashnode_t *cursor = NULL;
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

static int hashtable_insert_bucket(hashbucket_t *bkt, hashnode_t *node)
{
    int err = -1;

    if (NULL == hashtable_lookup_bucket(bkt, node->key, node->ksize))
    {
        list_add_tail(&(bkt->head), &node->list);
        bkt->nelems ++; 
        err = 0;
    }
    return err;
}

static int hashtable_rehash_bucket(hashtable_t *ht, hashbucket_t *bkt)
{
    hashnode_t *cursor;
    hashnode_t *next;
    unsigned int rehash;
    int err = 0;

    list_for_each_entry_safe(&bkt->head, cursor, next, list)
    {
        bkt->nelems --;
        list_remove(&cursor->list);
        rehash = hash_bkdr((char *)cursor->key, cursor->ksize, ht->resize); 
        err |= hashtable_insert_bucket(&(ht->rehash[rehash]), cursor);
    }
    return err;
}

static hashnode_t* 
hashtable_remove_from_bucket(hashbucket_t *bkt, void *key, unsigned int ksize)
{
    hashnode_t *node;
    node = hashtable_lookup_bucket(bkt, key, ksize);
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
static int hashtable_shrink_below_watermark(hashtable_t *ht)
{
    return ((ht->nelems < (ht->size / 4)) &&
            (ht->size > ht->min_size));
}

/*
 * Expand table when exceeding 75% load 
 * returns true if nelems > 0.75 * bkt->size
 */
static int hashtable_grow_above_watermark(hashtable_t *ht)
{
    return ((ht->nelems > (ht->size * 3 / 4)) &&
            (ht->max_size == 0 || ht->size < ht->max_size));
}

static int hashtable_rehash(hashtable_t *ht)
{
    int err = 0;
    unsigned int i;
    hashbucket_t *bkt;

    for (i = 0; i < ht->size; i++)
    {
        err |= hashtable_rehash_bucket(ht, &(ht->hash[i]));
    }
    if (0 == err)
    {
        bkt = ht->hash;

        ht->size = ht->resize;
        ht->hash = ht->rehash;
        ht->resize = 0;
        ht->rehash = NULL;
        hashtable_destroy_bucket(bkt);
    }
    return 0;
}

static int hashtable_shrink(hashtable_t *ht)
{
    hashbucket_t *rehash = NULL;
    int err = 0;

    rehash = hashtable_create_bucket(ht->size / 2);
    if (NULL != rehash)
    {
        ht->rehash = rehash;
        ht->resize = ht->size / 2;
        log_info("Hash resize: %d", ht->resize);
        err |= hashtable_rehash(ht);
    }
    return err;
}

static int hashtable_expand(hashtable_t *ht)
{
    hashbucket_t *rehash = NULL;
    int err = 0;

    rehash = hashtable_create_bucket(ht->size * 2);
    if (NULL != rehash)
    {
        ht->rehash = rehash;
        ht->resize = ht->size * 2;
        log_info("Hash resize: %d", ht->resize);
        err |= hashtable_rehash(ht);
    }

    return err;
}

hashtable_t* hashtable_create(unsigned int min_size, unsigned int max_size)
{
    hashtable_t *ht = NULL;
    
    ht = (hashtable_t *)malloc(sizeof(hashtable_t));
    if (NULL != ht)
    {
        memset(ht, 0, sizeof(hashtable_t));
        ht->min_size = min_size;
        ht->max_size = max_size;
        ht->elasticity = 16;
        
        ht->size = min_size;
        ht->hash = hashtable_create_bucket(min_size);
    }

    return ht;
}

void hashtable_destroy(hashtable_t *ht)
{
    return;
}

hashnode_t* hashtable_lookup(hashtable_t *ht, void *key, unsigned int ksize)
{
    unsigned int hash;
    hashnode_t *node;

    hash = hash_bkdr(key, ksize, ht->size);
    node = hashtable_lookup_bucket(&(ht->hash[hash]), key, ksize);
    if (NULL == node && NULL != ht->rehash)
    {
        hash = hash_bkdr((char *)key, ksize, ht->resize);
        node = hashtable_lookup_bucket(&(ht->rehash[hash]), key, ksize);
    }
    return node;
}

int hashtable_insert(hashtable_t *ht, void *key, unsigned int ksize)
{
    int err = 0;
    unsigned int hash;
    hashnode_t *node;

    node = hashtable_create_node(key, ksize);
    if (NULL == node)
    {
        return -1;
    }
    if (NULL != ht->rehash)
    {
        hash = hash_bkdr((char *)key, ksize, ht->resize);
        err = hashtable_insert_bucket(&(ht->rehash[hash]), node);
        if (0 == err)
        {
            ht->nelems ++;
        }
        /* else case is alread exist */
        return err;
    }

    hash = hash_bkdr((char *)key, ksize, ht->size);
    err = hashtable_insert_bucket(&(ht->hash[hash]), node);
    if (0 == err)
    {
        ht->nelems ++;
    }
    if (hashtable_grow_above_watermark(ht))
    {
        hashtable_expand(ht);
    }

    return err;
}

/* the return type is not reasonable */
void* hashtable_remove(hashtable_t *ht, void *key, unsigned int ksize)
{
    unsigned int hash;
    hashnode_t *node;
    void *value = NULL;

    hash = hash_bkdr(key, ksize, ht->size);
    node = hashtable_remove_from_bucket(&(ht->hash[hash]), key, ksize);
    if (NULL == node && NULL != ht->rehash)
    {
        hash = hash_bkdr((char *)key, ksize, ht->resize);
        node = hashtable_remove_from_bucket(&(ht->rehash[hash]), key, ksize);
    }

    if (NULL != node)
    {
        ht->nelems --;
        value = node->value;
        hashtable_destroy_node(node);
        node = NULL;
    }

    if (hashtable_shrink_below_watermark(ht))
    {
        hashtable_shrink(ht);
    }
    return value;
}

void hashtable_state(hashtable_t *ht)
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
