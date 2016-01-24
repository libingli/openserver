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

#define list_add_after(h, x)                                    \
    (x)->next = (h)->next;                                      \
    (x)->next->prev = x;                                        \
    (x)->prev = h;                                              \
    (h)->next = x

#define list_add_before(h, x)                                   \
    (x)->prev = (h)->prev;                                      \
    (x)->prev->next = x;                                        \
    (x)->next = h;                                              \
    (h)->prev = x

#define list_del(x)                                             \
    (x)->next->prev = (x)->prev;                                \
    (x)->prev->next = (x)->next;                                            

#define list_add_head      list_add_after

#define list_add_tail      list_add_before

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
 * @pos: the &struct list_t to use as a loop cursor.
 * @head: the head for your list.
 */
#define list_for_each(head, pos)                                \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos: the &struct list_t to use as a loop cursor.
 * @n: another &struct list_t to use as temporary storage
 * @head: the head for your list.
 */
#define list_for_each_safe(head, pos, n)                        \
    for (pos = (head)->next, n = pos->next; pos != (head);      \
        pos = n, n = pos->next)

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
 * @pos: the type * to cursor
 * @member: the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_for_each_entry    -    iterate over list of given type
 * @head: the head for your list.
 * @pos: the type * to use as a loop cursor.
 * @member: the name of the list_t within the struct.
 */
#define list_for_each_entry(head, pos, member)                  \
    for (pos = list_first_entry(head, typeof(*pos), member);    \
        &pos->member != (head);                                 \
        pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @head: the head for your list.
 * @pos: the type * to use as a loop cursor.
 * @n: another type * to use as temporary storage
 * @member: the name of the list_t within the struct.
 */
#define list_for_each_entry_safe(head, pos, n, member)          \
    for (pos = list_first_entry(head, typeof(*pos), member),    \
        n = list_next_entry(pos, member);                       \
        &pos->member != (head);                                 \
        pos = n, n = list_next_entry(n, member))

#endif /* _LIST_H_ */
