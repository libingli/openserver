/*********************************************************************
*   File Name     : hashtable_test.cc
*   Author        : libingli
*   Created Time  : Fri 29 Jan 2016 09:08:48 PM CST
**********************************************************************/
#include "gtest/gtest.h"
#include "hashtable.h"

static hashtable_t *ht = NULL;
static char g_test[] = "hash";
static char hash_value[150] = "helloXXXXXXXXXXXXXXXXXXXXXXXX";

TEST(hashtable, create)
{
    ht = hashtable_create(8, 128);
    EXPECT_TRUE(NULL != ht);
    EXPECT_EQ(8, ht->min_size);
    EXPECT_EQ(128, ht->max_size);
    return;
}

TEST(hashtable, insert)
{
    int err = 0;

    err = hashtable_insert(ht, g_test, sizeof(g_test));
    EXPECT_EQ(0, err);
}

TEST(hashtable, lookup)
{
    char test[] = "hash";
    hashnode_t *node;

    node = hashtable_lookup(ht, test, sizeof(test));
    EXPECT_TRUE(NULL != node);
}

TEST(hashtable, remove)
{
    char test[] = "hash";
    void *value;

    value = hashtable_remove(ht, test, sizeof(test));
    EXPECT_TRUE(NULL != value);
}

TEST(hashtable, insert_resize)
{
    int err = 0;
    unsigned int i = 0;

    for (i = 0; i < sizeof(hash_value)/sizeof(hash_value[0]); i++)
    {
        hash_value[i] = i;
    }
    for (i = 0; i < sizeof(hash_value)/sizeof(hash_value[0]); i++)
    {
        err = hashtable_insert(ht, &hash_value[i], sizeof(hash_value[i]));
        EXPECT_EQ(0, err);
    }
    EXPECT_EQ(150, ht->nelems);
    hashtable_state(ht);
}

TEST(hashtable, lookup_success)
{
    unsigned int i = 0;
    hashnode_t *node = NULL;

    for (i = 0; i < sizeof(hash_value)/sizeof(int); i++)
    {
        node = hashtable_lookup(ht, &hash_value[i], sizeof(hash_value[i]));
        EXPECT_TRUE(NULL != node);
    }
}

TEST(hashtable, lookup_failed)
{
    unsigned long i = 200;
    hashnode_t *node = NULL;

    node = hashtable_lookup(ht, &i, sizeof(i));
    EXPECT_TRUE(NULL == node);
}

TEST(hashtable, remove_resize)
{
    unsigned int i = 0;
    void *value;

    for (i = 0; i < sizeof(hash_value)/sizeof(hash_value[0]) - 10; i++)
    {
        value = hashtable_remove(ht, &hash_value[i], sizeof(hash_value[i]));
        EXPECT_TRUE(NULL != value);
    }
    EXPECT_EQ(10, ht->nelems);
    hashtable_state(ht);
}
