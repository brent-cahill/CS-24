/*============================================================================
 * Implementation of the RANDOM page replacement policy.
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
 * Policy Implementation
 */


/* Head and tail nodes of the queue. */
static node_t *head;
static node_t *tail;

/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using FIFO eviction policy.\n\n");
    
    head = NULL;
    tail = NULL;
    
    /* Return nonzero if initialization succeeded. */
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    head = NULL;
    tail = NULL;
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

    /* If the queue is empty */
    if (head == NULL && tail == NULL) {
        head = new;
    }
    else {
        tail->next = new;
    }

    /* Make the tail the new node */
    tail = new;
    return;
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    /* Still Do nothing! */
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a random
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    /* This will always be the head, as long as there is one */
    page_t victim = head->page;

    /* Clean up the queue and free memory where appropriate */
    node_t *old = head;
    head = head->next;
    free(old);
    old = NULL;

#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}