/*============================================================================
 * Implementation of the Clock/LRU page replacement policy.
 *
 * We don't mind if paging policies use malloc() and free(), just because it
 * keeps things simpler.  In real life, the pager would use the kernel memory
 * allocator to manage data structures, and it would endeavor to allocate and
 * release as little memory as possible to avoid making the kernel slow and
 * prone to memory fragmentation issues.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "vmpolicy.h"


/*============================================================================
 * "Node" Data Structure
 *
 * This data structure stores a page and a pointer to the next node with a
 page and pointer
 */

typedef struct node_t {
    page_t page;
    struct node_t *next;
} node_t;


/*============================================================================
 * "Queue" Data Structure
 *
 * This data structure holds all of the pages in a queue.
 */

typedef struct queue_t {
    node_t *head;
    node_t *tail;
} queue_t;


/*============================================================================
 * Policy Implementation
 */


/* The list of pages that are currently resident. */
static queue_t loaded;


/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using CLOCK/LRU eviction policy.\n\n");
    
    loaded.head = NULL;
    loaded.tail = NULL;
    
    /* Return nonzero if initialization succeeded. */
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    /* Do nothing! */
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    /* Allocate memory for a new page */
    node_t *new = (node_t *) malloc(sizeof(node_t));

    /* Add new node to end of queue */
    new->page = page;
    new->next = NULL;

    /* If the queue is empty vs not */
    if (loaded.head == NULL && loaded.tail == NULL) {
        loaded.head = new;
    }
    else {
        loaded.tail->next = new;
    }

    /* Make the tail the new node */
    loaded.tail = new;
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    /* Now we do something here */
    node_t *old_tail = loaded.tail;
    node_t *prev = NULL;
    node_t *curr = loaded.head;
    node_t *next;

    /* Get to the original tail */
    while (prev != old_tail) {
        if (is_page_accessed(curr->page)) {
            /* clear it */
            clear_page_accessed(curr->page);

            /* set permissions now */
            set_page_permission(curr->page, PAGEPERM_NONE);

            /* move it to the end of the queue */
            if (curr->next) {
                if (prev) {
                    /* In the case that we are not at the head */
                    prev->next = curr->next;
                }
                else {
                    /* Otherwise */
                    loaded.head = curr->next;
                }

                /* Update the next, tail, curr, prev, etc. */
                next = curr->next;
                curr->next = NULL;
                loaded.tail->next = curr;
                loaded.tail = curr;
                curr = next;
                prev = curr;
                curr = curr->next;
            }

            /* If we are at the end of the queue */
            else {
                break;
            }
        }

        /* If the page has not been accessed, then we continue to traverse */
        else {
            prev = curr;
            curr = curr->next;
        }
    }
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a random
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    /* This will always be the head, as long as there is one */
    page_t victim = loaded.head->page;

    /* Clean up the queue and free memory where appropriate */
    node_t *old = loaded.head;
    loaded.head = loaded.head->next;
    free(old);
    old = NULL;

#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}