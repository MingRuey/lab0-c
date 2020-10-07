#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q) {
        return;
    }

    while (q->head) {
        list_ele_t *target = q->head;
        q->head = q->head->next;
        free(target->value);
        free(target);
    }
    free(q);
}

/*
 * Attempt to create an element with value being s.
 * Return the pointer to element if successful.
 * Return NULL otherwise.
 */
static list_ele_t *loc_element(char *s)
{
    list_ele_t *new = malloc(sizeof(list_ele_t));
    if (!new) {
        return NULL;
    }

    size_t s_length = strlen(s) + 1;
    char *val = malloc(sizeof(char) * s_length);
    if (!val) {
        free(new);
        return NULL;
    }
    memcpy(val, s, sizeof(char) * s_length);
    new->value = val;
    new->next = NULL;
    return new;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }

    list_ele_t *newh = loc_element(s);
    if (!newh) {
        return false;
    }

    if (!q->size) {
        q->tail = newh;
    }
    newh->next = q->head;
    q->head = newh;
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }

    list_ele_t *newt = loc_element(s);
    if (!newt) {
        return false;
    }

    if (!q->size) {
        q->head = newt;
    } else {
        q->tail->next = newt;
    }
    q->tail = newt;
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head) {
        return false;
    }

    list_ele_t *target = q->head;
    q->head = q->head->next;
    q->size--;
    if (!q->size) {
        q->tail = NULL;
    }

    // TODO(memcpy to sp could case buffer overflow?)
    if (sp) {
        size_t v_length = strlen(target->value);
        if (v_length > bufsize) {
            memcpy(sp, target->value, bufsize - 1);
            sp[bufsize - 1] = '\0';
        } else {
            memcpy(sp, target->value, sizeof(char) * (v_length + 1));
        }
    }
    free(target->value);
    free(target);

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return !q ? 0 : q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head || q->size <= 1) {
        return;
    }

    // reverse the linked-list
    list_ele_t *next = q->head->next;
    list_ele_t *cur = q->head;
    while (next) {
        list_ele_t *tmp = next->next;
        next->next = cur;

        cur = next;
        next = tmp;
    }
    q->head->next = NULL;

    // swap head-tail
    list_ele_t *tmp = q->head;
    q->head = q->tail;
    q->tail = tmp;
}


/*
 * Merge two list and return the new head.
 */
static list_ele_t *merge(list_ele_t *head1, list_ele_t *head2)
{
    list_ele_t *merged = NULL;
    list_ele_t *cursor = NULL;
    while (head1 && head2) {
        list_ele_t **head =
            strcmp(head1->value, head2->value) <= 0 ? &head1 : &head2;
        if (!cursor) {
            merged = *head;
            cursor = *head;
        } else {
            cursor->next = *head;
            cursor = cursor->next;
        }
        *head = (*head)->next;
    }

    if (head1) {
        cursor->next = head1;
    } else if (head2) {
        cursor->next = head2;
    }
    return merged;
}

/*
 * Sort the linked list with known length.
 */
static void recur_sort(list_ele_t **target, int length)
{
    if (length <= 1) {
        return;
    }

    list_ele_t *lhead = *target, *rhead = *target;
    int halflen = length / 2;
    for (int i = halflen; i > 1; i--) {
        rhead = rhead->next;
    }
    list_ele_t *tmp = rhead;
    rhead = rhead->next;
    tmp->next = NULL;

    recur_sort(&lhead, halflen);
    recur_sort(&rhead, length - halflen);
    *target = merge(lhead, rhead);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q->size || q->size == 1) {
        return;
    }

    recur_sort(&q->head, q->size);
    list_ele_t *ele = q->head;
    for (int l = q->size; l > 1; l--) {
        ele = ele->next;
    }
    q->tail = ele;
}
