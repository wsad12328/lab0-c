#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
    }
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe(entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_element = malloc(sizeof(element_t));

    if (!new_element)
        return false;

    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }
    INIT_LIST_HEAD(&new_element->list);
    list_add(&new_element->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_element = malloc(sizeof(element_t));

    if (!new_element)
        return false;

    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }
    INIT_LIST_HEAD(&new_element->list);
    list_add_tail(&new_element->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *first = head->next;
    element_t *entry = list_entry(first, element_t, list);

    if (sp && bufsize > 0) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(first);

    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *last = head->prev;
    element_t *entry = list_entry(last, element_t, list);

    if (sp && bufsize > 0) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(last);

    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each(li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    int n = q_size(head);

    if (n == 0)
        return false;

    int mid = n / 2;

    element_t *entry, *safe;
    int count = 0;

    list_for_each_entry_safe(entry, safe, head, list) {
        if (count == mid) {
            list_del(&entry->list);
            q_release_element(entry);
            break;
        }
        count++;
    }

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    bool deleted = false;
    struct list_head *curr = head->next;

    while (curr != head) {
        const element_t *curr_entry = list_entry(curr, element_t, list);
        struct list_head *next = curr->next;

        // Check if current node has duplicates
        if (next != head) {
            const element_t *next_entry = list_entry(next, element_t, list);

            if (strcmp(curr_entry->value, next_entry->value) == 0) {
                // Found duplicates, remove all nodes with this value
                char *dup_val = strdup(curr_entry->value);

                while (curr != head) {
                    element_t *temp_entry = list_entry(curr, element_t, list);
                    struct list_head *temp_next = curr->next;

                    if (strcmp(temp_entry->value, dup_val) == 0) {
                        list_del(curr);
                        q_release_element(temp_entry);
                        deleted = true;
                        curr = temp_next;
                    } else {
                        break;
                    }
                }

                free(dup_val);
                continue;
            }
        }

        curr = next;
    }

    return deleted;
}


/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;

    struct list_head *curr = head->next;

    while (curr != head && curr->next != head) {
        struct list_head *next = curr->next;

        next->prev = curr->prev;
        curr->prev = next;

        curr->next = next->next;
        next->next = curr;
        next->prev->next = next;
        curr->next->prev = curr;
        curr = curr->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *curr = head;

    do {
        struct list_head *temp = curr->next;
        curr->next = curr->prev;
        curr->prev = temp;
        curr = temp;
    } while (curr != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k <= 1)
        return;

    int total_nodes = q_size(head);
    if (total_nodes < k)
        return;

    struct list_head *curr = head->next;

    while (curr != head) {
        // Check if we have k nodes remaining
        int count = 0;
        struct list_head *check = curr;
        while (check != head) {
            check = check->next;
            count++;
        }
        if (count < k)
            break;

        struct list_head *group_start = curr;
        struct list_head *group_end = curr;

        for (int i = 0; i < k - 1; i++)
            group_end = group_end->next;

        struct list_head *reverse_curr = group_start;
        struct list_head *prev_group = group_start->prev;
        struct list_head *next_group = group_end->next;

        for (int i = 0; i < k; i++) {
            struct list_head *temp = reverse_curr->next;
            reverse_curr->next = reverse_curr->prev;
            reverse_curr->prev = temp;
            reverse_curr = temp;
        }

        group_start->next = next_group;
        prev_group->next = group_end;
        group_end->prev = prev_group;
        next_group->prev = group_start;

        curr = next_group;
    }
}

void merge(struct list_head *l1,
           struct list_head *l2,
           struct list_head *result,
           bool descend)
{
    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *l1_entry = list_first_entry(l1, element_t, list);
        element_t *l2_entry = list_first_entry(l2, element_t, list);

        int cmp = strcmp(l1_entry->value, l2_entry->value);

        if (descend ? cmp >= 0 : cmp <= 0) {
            list_move_tail(&l1_entry->list, result);
        } else {
            list_move_tail(&l2_entry->list, result);
        }
    }

    if (!list_empty(l1))
        list_splice_tail(l1, result);


    if (!list_empty(l2))
        list_splice_tail(l2, result);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *fast = head->next;
    struct list_head *slow = head->next;

    LIST_HEAD(left);
    LIST_HEAD(right);

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    struct list_head *current = head->next;

    while (current != slow) {
        struct list_head *next = current->next;
        list_move_tail(current, &left);
        current = next;
    }

    list_splice_tail_init(head, &right);

    q_sort(&left, descend);
    q_sort(&right, descend);
    merge(&left, &right, head, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    LIST_HEAD(stack);

    element_t *entry, *safe;
    list_for_each_entry_safe(entry, safe, head, list) {
        while (!list_empty(&stack)) {
            element_t *top = list_last_entry(&stack, element_t, list);
            if (strcmp(entry->value, top->value) < 0) {
                list_del(&top->list);
                q_release_element(top);
            } else {
                break;
            }
        }

        list_del(&entry->list);
        list_add_tail(&entry->list, &stack);
    }

    if (!list_empty(&stack))
        list_splice_tail_init(&stack, head);

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    LIST_HEAD(stack);

    element_t *entry, *safe;
    list_for_each_entry_safe(entry, safe, head, list) {
        while (!list_empty(&stack)) {
            element_t *top = list_last_entry(&stack, element_t, list);
            if (strcmp(entry->value, top->value) > 0) {
                list_del(&top->list);
                q_release_element(top);
            } else {
                break;
            }
        }

        list_del(&entry->list);
        list_add_tail(&entry->list, &stack);
    }

    if (!list_empty(&stack))
        list_splice_tail_init(&stack, head);

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))  // Only one queue
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);

    queue_contex_t *first_queue = list_first_entry(head, queue_contex_t, chain);
    struct list_head *result = first_queue->q;

    // Start from the second queue using next pointer
    struct list_head *current = first_queue->chain.next;

    while (current != head) {
        queue_contex_t *entry = list_entry(current, queue_contex_t, chain);
        struct list_head *next =
            current->next;  // Save next before modification

        LIST_HEAD(temp_result);
        LIST_HEAD(temp_entry);
        list_splice_tail_init(result, &temp_result);
        list_splice_tail_init(entry->q, &temp_entry);

        merge(&temp_result, &temp_entry, result, descend);
        INIT_LIST_HEAD(entry->q);

        current = next;
    }

    return q_size(result);
}