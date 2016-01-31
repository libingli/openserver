#ifndef _LIST_H_
#define _LIST_H_

#include <stddef.h>

typedef struct tag_list{
    struct tag_list  *prev;
    struct tag_list  *next;
}list_t;

#define list_init(x)                                            \
    (x)->prev = (x);                                            \
    (x)->next = (x) 

#define list_empty(h)                                           \
    (h == (h)->prev)

#define list_add_head(h, x)                                    \
    (x)->next = (h)->next;                                      \
    (x)->next->prev = x;                                        \
    (x)->prev = h;                                              \
    (h)->next = x

#define list_add_tail(h, x)                                   \
    (x)->prev = (h)->prev;                                      \
    (x)->prev->next = x;                                        \
    (x)->next = h;                                              \
    (h)->prev = x

#define list_remove(x)                                             \
    (x)->next->prev = (x)->prev;                                \
    (x)->prev->next = (x)->next;                                            

#define list_add_after          list_add_head

#define list_add_before         list_add_tail

#define list_first(h)                                           \
    (h)->next

#define list_last(h)                                            \
    (h)->prev

#define list_sentinel(h)                                        \
    (h)

#define list_next(ptr)                                          \
    (ptr)->next

#define list_prev(ptr)                                          \
    (ptr)->prev

/**
 * list_for_each    -    iterate over a list
 * @cursor: the &struct list_t to use as a loop cursor.
 * @head: the head for your list.
 */
#define list_for_each(head, cursor)                                \
    for (cursor = (head)->next; cursor != (head); cursor = cursor->next)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @cursor: the &struct list_t to use as a loop cursor.
 * @n: another &struct list_t to use as temporary storage
 * @head: the head for your list.
 */
#define list_for_each_safe(head, cursor, n)                        \
    for (cursor = (head)->next, n = cursor->next; cursor != (head);      \
        cursor = n, n = cursor->next)

#define list_entry(ptr, type, member)                           \
    (type *) ((u_char *)(typeof( ((type *)0)->member ) *)ptr - offsetof(type, member))

/**
 * list_first_entry - get the first element from a list
 * @head: the list head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_t within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(head, type, member)                    \
    list_entry((head)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @head: the list head to take the element from.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_t within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(head, type, member)                     \
    list_entry((head)->prev, type, member)

/**
 * list_next_entry - get the next element in list
 * @cursor: the type * to cursor
 * @member: the name of the list_head within the struct.
 */
#define list_next_entry(cursor, member) \
    list_entry((cursor)->member.next, typeof(*(cursor)), member)

/**
 * list_for_each_entry    -    iterate over list of given type
 * @head: the head for your list.
 * @cursor: the type * to use as a loop cursor.
 * @member: the name of the list_t within the struct.
 */
#define list_for_each_entry(head, cursor, member)                  \
    for (cursor = list_first_entry(head, typeof(*cursor), member);    \
        &cursor->member != (head);                                 \
        cursor = list_next_entry(cursor, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @head: the head for your list.
 * @cursor: the type * to use as a loop cursor, can be free in the loop.
 * @n: another type * to use as temporary storage
 * @member: the name of the list_t within the struct.
 */
#define list_for_each_entry_safe(head, cursor, next, member)          \
    for (cursor = list_first_entry(head, typeof(*cursor), member),    \
        next = list_next_entry(cursor, member);                       \
        &cursor->member != (head);                                 \
        cursor = next, next = list_next_entry(next, member))

#endif /* _LIST_H_ */
