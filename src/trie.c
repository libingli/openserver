/*************************************************************************
 * File     : trie.c
 * Author   : libingli
 * Created  : Fri 15 Jan 2016 06:15:53 AM CST
*************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hash.h"
#include "trie.h"
#include "log.h"

static void trie_destroy_branch(trie_node_s *branch[], unsigned int num);

static char trie_compare_element(const char *element, const char *str, unsigned int len)
{
    char equal = 0;

    if (strlen(element) == len)
    {
        equal = strncmp(element, str, len);
        equal = (equal == 0); 
    }
    return equal;
}

static trie_node_s* trie_create_node(const char *string, int length)
{
    trie_node_s *node = NULL;
    char *element = NULL;
    int size = length + 1;

    element = (char *)malloc(size);
    if (NULL != element)
    {
        node = (trie_node_s *)malloc(sizeof(trie_node_s));
    }
    if (NULL == node)
    {
        free(element);
        log_err("Out of memory.");
        return node;
    }
    memset(element, 0, size);
    memset(node, 0, sizeof(trie_node_s));
    node->element = element;
    snprintf(element, size, "%s", string);
    list_init(&node->list);
    node->branches = NULL;

    return node;
}

static void trie_destroy_node(trie_node_s *node, unsigned int num)
{
    trie_node_s *b = NULL;
    trie_node_s *n = NULL;
  
    assert(NULL != node);  
    trie_destroy_branch(node->branches, num);
    free(node->element);
    list_for_each_entry_safe(&(node->list), b, n, list)
    {
        list_remove(&b->list);
        trie_destroy_branch(b->branches, num);
    }
    free(node);
    return;
}

static trie_node_s** trie_create_branch(unsigned int num)
{
    trie_node_s **branch = NULL;
    
    branch = (trie_node_s **)malloc(sizeof(trie_node_s *) * num);
    if (NULL != branch)
    {
        //log_dbg("malloc branch size: %d", sizeof(trie_node_s *) * num);
        memset(branch, 0, sizeof(trie_node_s *) * num);
    }

    return branch;
}

static void trie_destroy_branch(trie_node_s *branch[], unsigned int num)
{
    unsigned int i;

    if (NULL == branch)
    {
        return;
    }
   
    /* TODO [enhance]:Prevent stack overflow, without recursion */ 
    for (i = 0; i < num; i++)
    {
        if (NULL != branch[i])
        {
            trie_destroy_node(branch[i], num);
        }
    }

    return;
}

static trie_node_s*** 
trie_insert_branch(trie_root_s *root, trie_node_s ***branch, const char *element, int length)
{
    trie_node_s *node = NULL;
    trie_node_s *cursor = NULL;
    trie_node_s **b = NULL;
    unsigned int hash = 0;

    hash = hash_bkdr(element, length, root->branch_num);
    if (NULL == branch || NULL == *branch)
    {
        b = trie_create_branch(root->branch_num);
        if (NULL == b)
        {
            log_err("Out of memory. %s: %d", __FILE__, __LINE__);
            return NULL;
        }
        *branch = b;
    }
   
    node = (*branch)[hash]; 
    if (NULL == node)
    {
        node = trie_create_node(element, length);
        (*branch)[hash] = node;
        root->elements ++;
    }
    /* Hash collision */
    else
    {
        if (trie_compare_element(node->element, element, length))
        {
            return &(node->branches);
        }

        list_for_each_entry(&(node->list), cursor, list)
        {
            assert(NULL != cursor);
            if (trie_compare_element(cursor->element, element, length))
            {
                break;
            }
        }
        /* no exist */
        if (&(cursor->list) == &(node->list))
        {
            node = trie_create_node(element, length);
            if (NULL != node)
            {
                root->elements ++;
                root->collision ++;
                (*branch)[hash]->list_num ++;
                list_add_tail(&((*branch)[hash]->list), &node->list);
            }
        }
        else
        {
            node = cursor;
        }
    }
    if (NULL == node)
    {
        log_err("Out of memory. %s: %d", __FILE__, __LINE__);
        return NULL;
    }
    
    return &(node->branches);
}

int trie_insert(trie_root_s *root, const char *string)
{
    trie_node_s ***branch = NULL;
    const char *start = NULL;
    const char *end = NULL;
    int length = 0;
    char done = 0;
    int err = 0;

    start = string;
    end = string;
    branch = &(root->branches);
    while (0 == done)
    {
        end = strchr(start, root->separater);
        if (NULL == end)
        {
            done = 1;
            end = strchr(string, '\0');
        }
        length = end - start;
        branch = trie_insert_branch(root, branch, start, length);
        if (NULL == branch && 0 == done)
        {
            err = -1;
            break;
        }        
        start = end + 1;
    }
    return err;
}

void trie_remove(trie_root_s *root, const char *string)
{
    return;
}

trie_node_s** 
trie_find_branch(trie_root_s *root, trie_node_s **branch, 
                 const char *element, int length, char *hit)
{
    trie_node_s *node = NULL;
    unsigned int hash = 0;
    int status = -1;

    hash = hash_bkdr(element, length, root->branch_num);
    if (NULL == branch || NULL == branch[hash])
    {
        *hit = 0;
        return NULL;
    }
    
    node = branch[hash];
    status = trie_compare_element(branch[hash]->element, element, length);
    if (0 != status)
    {
        *hit = 1;
        return branch[hash]->branches;
    }
    
    list_for_each_entry(&(branch[hash]->list), node, list)
    {
        assert(NULL != node);
        if (trie_compare_element(node->element, element, length))
        {
            *hit = 1;
            break;
        }
    }

    if (1 == *hit)
    {
        return node->branches;
    }
    return NULL;
}

char trie_find(trie_root_s *root, const char *string)
{
    trie_node_s **branch = NULL;
    const char *start = NULL;
    const char *end = NULL;
    int length = 0;
    char done = 0;
    char hit = 0;
  
    start = string;
    end = string;
    branch = root->branches;
    while (!done)
    {
        hit = 0;
        end = strchr(start, root->separater);
        if (NULL == end)
        {
            done = 1;
            end = strchr(string, '\0');
        }
        length = end - start;
        branch = trie_find_branch(root, branch, start, length, &hit);
        if (0 == hit)
        {
            break;
        }
        start = end + 1;
    }

    return hit;
}

trie_root_s* trie_create(unsigned int branch_num, char separater)
{
    trie_root_s *root = NULL;

    root = (trie_root_s *)malloc(sizeof(trie_root_s));
    if (NULL != root)
    {
        //log_dbg("malloc size: %d", sizeof(trie_root_s));
        memset(root, 0, sizeof(trie_root_s));
        root->branch_num = branch_num;
        root->separater = separater;
    }

    return root;
}

void trie_destroy(trie_root_s *root)
{
    if (NULL != root && NULL != root->branches)
    {
        trie_destroy_branch(root->branches, root->branch_num);
        free(root);
        root = NULL;
    }
    return;
}

