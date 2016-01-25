/*************************************************************************
 * File     : trie_test.c
 * Author   : libingli
 * Created  : Fri 15 Jan 2016 08:06:28 AM CST
*************************************************************************/
#include "gtest/gtest.h"
#include "trie.h"
#include "hash.h"
#include "log.h"

#define TRIE_INSERT_FILE    "/home/li/openserver/gtest/test/trie_insert.txt"
#define TRIE_FIND_FILE      "/home/li/openserver/gtest/test/trie_find.txt"
#define MAX_LINE_LEN        200

static int test_branch_num = 1;
trie_root_s *root = NULL;

TEST(trie, create)
{
    root = trie_create(test_branch_num, '/');
    EXPECT_TRUE(NULL != root);
    EXPECT_EQ(test_branch_num, root->branch_num);
    return;
}

TEST(trie, insert)
{
    int err = 0;
    unsigned int hash1 = 0;
    unsigned int hash2 = 0;
    unsigned int hash3 = 0;
    unsigned int hash4 = 0;

    ASSERT_TRUE(NULL != root);
    EXPECT_EQ('/', root->separater);
    err = trie_insert(root, "www.abc.cc/def/hig/index.html");
    EXPECT_EQ(0, err);

    hash1 = hash_bkdr("www.abc.cc", strlen("www.abc.cc"), test_branch_num);
    hash2 = hash_bkdr("def", strlen("def"), test_branch_num);
    hash3 = hash_bkdr("hig", strlen("hig"), test_branch_num);
    hash4 = hash_bkdr("index.html", strlen("index.html"), test_branch_num);
   
    EXPECT_TRUE(NULL != root->branches);
    EXPECT_TRUE(NULL != root->branches[hash1]);
    EXPECT_EQ(0, strcmp(root->branches[hash1]->element, "www.abc.cc"));
    
    EXPECT_TRUE(NULL != root->branches[hash1]->branches);
    EXPECT_TRUE(NULL != root->branches[hash1]->branches[hash2]);
    EXPECT_EQ(0, strcmp(root->branches[hash1]->branches[hash2]->element, "def"));

    EXPECT_TRUE(NULL != root->branches[hash1]->branches[hash2]->branches);
    EXPECT_TRUE(NULL != root->branches[hash1]->branches[hash2]->branches[hash3]);
    EXPECT_EQ(0, strcmp(root->branches[hash1]->branches[hash2]->branches[hash3]->element, "hig"));
    
    EXPECT_TRUE(NULL != root->branches[hash1]->branches[hash2]->branches[hash3]->branches);
    EXPECT_TRUE(NULL != root->branches[hash1]->branches[hash2]->branches[hash3]->branches[hash4]);
    EXPECT_EQ(0, strcmp(root->branches[hash1]->branches[hash2]->branches[hash3]->branches[hash4]->element, "index.html"));
   
    return;
}

TEST(trie, find)
{
    char hit = 0;
    int err = 0;

    hit = trie_find(root, "www.abc.cc");
    EXPECT_EQ(1, hit);
    hit = trie_find(root, "www.abc.cc/def/hig/index.html");
    EXPECT_EQ(1, hit);
    hit = trie_find(root, "www.abc.cc/def/hig/index");
    EXPECT_EQ(0, hit);
   
    err = trie_insert(root, "www.abc.cc/index.html");
    EXPECT_EQ(0, err);
    hit = trie_find(root, "www.abc.cc/index.html");
    EXPECT_EQ(1, hit);
    hit = trie_find(root, "www.abc.cc/index.html/def");
    EXPECT_EQ(0, hit);
    
    hit = trie_find(root, "www.abc.cc/def/hig/index.html");
    EXPECT_EQ(1, hit);
    hit = trie_find(root, "www.abc.cc/def/hig/index");
    EXPECT_EQ(0, hit);
    return;
}

TEST(trie, batch_insert)
{
    FILE *file = NULL;
    char line[MAX_LINE_LEN];
    int err = 0;
    
    file = fopen(TRIE_INSERT_FILE, "r");
    ASSERT_TRUE(NULL != file);
   
    while (fgets(line, sizeof(line), file))
    {
        err = trie_insert(root, line);
        EXPECT_EQ(0, err);
    }
    
    fclose(file);

    return;
}

TEST(trie, batch_find)
{
    FILE *file = NULL;
    char line[MAX_LINE_LEN];
    int hit = 0;
    int i = 0;

    file = fopen(TRIE_FIND_FILE, "r");
    ASSERT_TRUE(NULL != file);
   
    log_info("%s: Time start...", __FUNCTION__); 
    while (fgets(line, sizeof(line), file))
    {
        hit += trie_find(root, line);
        i++;
    }
    log_info("%s: Time end..., hit %d/%d", __FUNCTION__, hit, i); 
    
    fclose(file);
   

    return;
}
