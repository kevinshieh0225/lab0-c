#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return NULL;
    struct list_head *head = &node->list;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *ptr = l->next;
    while (ptr != l) {
        element_t *node = container_of(ptr, element_t, list);
        ptr = ptr->next;
        q_release_element(node);
    }
    element_t *node = container_of(l, element_t, list);
    free(node);
}

/* Insert an element at head of queue 
 * 
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */

bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    int charsize = strlen(s);
    char *value = malloc(charsize + 1);
    if (!value) {
        free(node);
        return false;
    }
    memcpy(value, s, charsize);
    value[charsize] = '\0';
    node->value = value;
    list_add(&node->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    int charsize = strlen(s);
    char *value = malloc(charsize + 1);
    if (!value) {
        free(node);
        return false;
    }
    memcpy(value, s, charsize);
    value[charsize] = '\0';
    node->value = value;
    list_add_tail(&node->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = container_of(head->next, element_t, list);
    if (sp) {
        memcpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->next);
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = container_of(head->prev, element_t, list);
    if (sp) {
        memcpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);
    return node;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int num = 0;
    struct list_head *node;
    if (head)
        list_for_each (node, head)
            num += 1;
    return num;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 * Ref: https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
 */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *slowptr = head->next;
    for (struct list_head *fastptr = head->next;
         fastptr != head && fastptr->next != head;
         fastptr = fastptr->next->next) {
        slowptr = slowptr->next;
    }
    element_t *node = container_of(slowptr, element_t, list);
    list_del(slowptr);
    q_release_element(node);
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 * Ref: https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
 */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *ckp = head->next;
    element_t *ckpnode = container_of(ckp, element_t, list);
    for (struct list_head *ptr = ckp->next; ptr != head; ptr = ckp->next) {
        element_t *ptrnode = container_of(ptr, element_t, list);
        if (strcmp(ckpnode->value, ptrnode->value) == 0) {
            list_del(ptr);
            q_release_element(ptrnode);
        } else {
            ckp = ptr;
            ckpnode = container_of(ckp, element_t, list);
        }
    }
    return true;
}

/* 
 * Swap every two adjacent nodes
 * 
 * Attempt to swap every two adjacent nodes.
 * Ref: https://leetcode.com/problems/swap-nodes-in-pairs/
 */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *ptr = head;
    while (ptr->next != head && ptr->next->next != head) {
        // swap next pointer
        struct list_head *tmp = ptr->next->next->next;
        ptr->next->next->next = ptr->next;
        ptr->next = ptr->next->next;
        ptr->next->next->next = tmp;
        // swap prev pointer
        ptr->next->next->next->prev = ptr->next->next;
        ptr->next->next->prev = ptr->next;
        ptr->next->prev = ptr;
        ptr = ptr->next->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    struct list_head *ptr = head;
    if (ptr && !list_empty(ptr)) {
        do {
            struct list_head *tmp = ptr->prev;
            ptr->prev = ptr->next;
            ptr->next = tmp;
            ptr = ptr->prev;
        } while (ptr != head);
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
struct list_head *mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **cur;
    for (cur = NULL; L1 && L2; *cur = (*cur)->next) {
        // compare accending pair by pair
        element_t *node1 = container_of(L1, element_t, list);
        element_t *node2 = container_of(L2, element_t, list);
        cur = (strcmp(node1->value, node2->value) <= 0) ? &L1 : &L2;
        *ptr = *cur;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    return head;
}

static struct list_head *mergesort_list(struct list_head *head)
{
    if (!head->next)
        return head;
    struct list_head *slow = head;
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow->next;
    slow->next = NULL;

    struct list_head *left = mergesort_list(head), *right = mergesort_list(mid);
    return mergeTwoLists(left, right);
}

void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = mergesort_list(head->next);
    // reassign the prev ptr
    struct list_head *ptr = head;
    for (; ptr->next; ptr = ptr->next)
        ptr->next->prev = ptr;
    ptr->next = head;
    head->prev = ptr;
}

// iterate version
// void q_sort(struct list_head *head)
// {
//     if (!head || list_empty(head))
//         return;
//     head->prev->next = NULL;

//     int top = 0;
//     int listsSize = 0;
//     struct list_head *stack[1000] = {NULL};
//     struct list_head *lists[150000] = {NULL};

//     stack[top] = head->next;
//     // divide to single node
//     while (top >= 0) {
//         // printf("[%d %d]",top ,listsSize);
//         struct list_head *left = stack[top--];
//         if (left) {
//             struct list_head *slow = left;
//             struct list_head *fast;
//             for (fast = left->next; fast && fast->next;
//                   fast = fast->next->next)
//                 slow = slow->next;
//             struct list_head *right = slow->next;
//             slow->next = NULL;

//             stack[++top] = left;
//             stack[++top] = right;
//         } else
//             lists[listsSize++] = stack[top--];
//     }
//     // printf("end\n");
//     // merge K sorted lists
//     while (listsSize > 1) {
//         for (int i = 0, j = listsSize - 1; i < j; i++, j--)
//             lists[i] = mergeTwoLists(lists[i], lists[j]);
//         listsSize = (listsSize + 1) / 2;
//     }
//     // reassign the prev ptr
//     head->next = lists[0];
//     struct list_head *ptr = head;
//     for (; ptr->next; ptr = ptr->next)
//         ptr->next->prev = ptr;
//     ptr->next = head;
//     head->prev = ptr;
// }
