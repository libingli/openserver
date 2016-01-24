/*************************************************************************
 * File     : trie.h
 * Author   : libingli
 * Created  : Fri 15 Jan 2016 06:16:01 AM CST
*************************************************************************/
#ifndef _TRIE_H_
#define _TRIE_H_

#include "list.h"

typedef struct trie_node {
    char                    *element;
    unsigned int            list_num;
    list_t                  list;       /* Hash collision list */
    struct trie_node        *parent;
    struct trie_node        **branches; /* *branches[] */ 
}trie_node_s;

typedef struct trie_root {
    char            separater;
    unsigned int    elements;           /* element sum */
    unsigned int    collision;          /* hash collision number */
    int             branch_num;
    trie_node_s     **branches;         /* *branches[] */
}trie_root_s;

int trie_insert(trie_root_s *root, const char *string);
void trie_remove(trie_root_s *root, const char *string);
char trie_find(trie_root_s *root, const char *string);

trie_root_s* trie_create(unsigned int branch_num, char separater);
void trie_destroy(trie_root_s *root);

#endif /* _TRIE_H_ */
