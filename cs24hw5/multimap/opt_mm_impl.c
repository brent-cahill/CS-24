#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multimap.h"


/*============================================================================
 * TYPES
 *
 *   These types are defined in the implementation file so that they can
 *   be kept hidden to code outside this source file.  This is not for any
 *   security reason, but rather just so we can enforce that our testing
 *   programs are generic and don't have any access to implementation details.
 *============================================================================*/


/* Represents a key and its associated values in the multimap, as well as
 * pointers to the left and right child nodes in the multimap. */
typedef struct multimap_node {
    /* The key-value that this multimap node represents. */
    int key;

    /* Array of the values stored for each key */
    int *values;

    /* Maximum size of the above array */
    int size;

    /* Final index to use to add new vals to a given key. */
    int fin_i;

    /* The left child of the multimap node.  This will reference nodes that
     * hold keys that are strictly less than this node's key.
     */
    struct multimap_node *left_child;

    /* The right child of the multimap node.  This will reference nodes that
     * hold keys that are strictly greater than this node's key.
     */
    struct multimap_node *right_child;
} multimap_node;


/* The entry-point of the multimap data structure. */
struct multimap {
    multimap_node *root;
};

void **mem_arry = NULL; /* Start of the mem array */
int arry_count = 0; /* number of memory banks in use */
int tot_arry = 0; /* Total number of memory banks */
int nodes = 0; /* Number of nodes in bank */
int mem_size = 1000000; /* size of a memory bank, initialized to 1M */



/*============================================================================
 * HELPER FUNCTION DECLARATIONS
 *
 *   Declarations of helper functions that are local to this module.  Again,
 *   these are not visible outside of this module.
 *============================================================================*/

multimap_node * alloc_mm_node();

multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found);

void free_multimap_node(multimap_node *node);

/* New functions */
void new_mem_bank();
void bank_resize();
void val_resize(multimap_node *n);

/*============================================================================
 * FUNCTION IMPLEMENTATIONS
 *============================================================================*/

/* Allocates a multimap node, and zeros out its contents so that we know what
 * the initial value of everything will be.
 */

/* Creates a new memory bank */

void new_mem_bank() {
    if (arry_count == tot_arry) {
        bank_resize();
    }
    mem_arry[arry_count] = malloc(mem_size * sizeof(multimap_node));
    arry_count++;
    nodes = 0;
}

/* Either creates a bank, if there exist none, or doubles the size of the
 * array of banks.
 */
void bank_resize() {
    if (tot_arry) {
        void **update_arry = malloc(2 * tot_arry * sizeof(void *));
        for (int i = 0; i < tot_arry; i++) {
            update_arry[i] = mem_arry[i];
        }
        free(mem_arry);
        mem_arry = update_arry;
        tot_arry *= 2;
    }
    else {
        tot_arry = 1;
        mem_arry = malloc(tot_arry * sizeof(void *));
    }
}

/* Either creates a values array, if there exist none, or doubles the size of
 * the array of values.
 */
void val_resize(multimap_node *n) {
    if (n->size) {
        int *update_vals = malloc(2 * n->size * sizeof(int));
        
        for (int i = 0; i < n->size; i++) {
            update_vals[i] = n->values[i];
        }

        free(n->values);
        n->values = update_vals;
        n->size *= 2;
    }
    else {
        n->size = 1;
        n->values = malloc(n->size * sizeof(int));
    }
}


multimap_node * alloc_mm_node() {
    if (nodes * sizeof(multimap_node) >= mem_size || arry_count == 0) {
        new_mem_bank();
    }

    multimap_node *node = mem_arry[arry_count - 1] + (nodes *
        sizeof(multimap_node));

    nodes++;
    bzero(node, sizeof(multimap_node));

    return node;
}


/* This helper function searches for the multimap node that contains the
 * specified key.  If such a node doesn't exist, the function can initialize
 * a new node and add this into the structure, or it will simply return NULL.
 * The one exception is the root - if the root is NULL then the function will
 * return a new root node.
 */
multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found) {
    multimap_node *node;

    /* If the entire multimap is empty, the root will be NULL. */
    if (root == NULL) {
        if (create_if_not_found) {
            root = alloc_mm_node();
            root->key = key;
        }
        return root;
    }

    /* Now we know the multimap has at least a root node, so start there. */
    node = root;
    while (1) {
        if (node->key == key)
            break;

        if (node->key > key) {   /* Follow left child */
            if (node->left_child == NULL && create_if_not_found) {
                /* No left child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;

                node->left_child = new;
            }
            node = node->left_child;
        }
        else {                   /* Follow right child */
            if (node->right_child == NULL && create_if_not_found) {
                /* No right child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;

                node->right_child = new;
            }
            node = node->right_child;
        }

        if (node == NULL)
            break;
    }

    return node;
}


/* This helper function frees all values in a multimap node's value array. */
void free_multimap_values(int *values) {
    if (values != NULL) {
        free(values);
    }
}


/* This helper function frees a multimap node, including its children and
 * value array.
 */
void free_multimap_node(multimap_node *node) {
    if (node == NULL)
        return;

    /* Free the children first. */
    free_multimap_node(node->left_child);
    free_multimap_node(node->right_child);

    /* Free the list of values. */
    free_multimap_values(node->values);

#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(node, sizeof(multimap_node));
#endif
}


/* Initialize a multimap data structure. */
multimap * init_multimap() {
    multimap *mm = malloc(sizeof(multimap));
    mm->root = NULL;
    return mm;
}


/* Release all dynamically allocated memory associated with the multimap
 * data structure.
 */
void clear_multimap(multimap *mm) {
    assert(mm != NULL);
    free_multimap_node(mm->root);
    mm->root = NULL;

    /* Free the banks in the array */
    for (int i = 0; i < arry_count; i++)
    {
        free(mem_arry[i]);
    }

    mem_arry = NULL;
    arry_count = 0;
    tot_arry = 0;
    nodes = 0;
}


/* Adds the specified (key, value) pair to the multimap. */
void mm_add_value(multimap *mm, int key, int value) {
    multimap_node *node;

    assert(mm != NULL);

    /* Look up the node with the specified key.  Create if not found. */
    node = find_mm_node(mm->root, key, /* create */ 1);
    if (mm->root == NULL)
        mm->root = node;

    assert(node != NULL);
    assert(node->key == key);

    /* resize the value array dependent on if there is enough space */

    if (node->fin_i == node->size) {
        val_resize(node);
    }

    /* Now we add the new value */
    node->values[node->fin_i] = value;
    node->fin_i++;
}


/* Returns nonzero if the multimap contains the specified key-value, zero
 * otherwise.
 */
int mm_contains_key(multimap *mm, int key) {
    return find_mm_node(mm->root, key, /* create */ 0) != NULL;
}


/* Returns nonzero if the multimap contains the specified (key, value) pair,
 * zero otherwise.
 */
int mm_contains_pair(multimap *mm, int key, int value) {
    multimap_node *node;
    int i_ = 0;

    node = find_mm_node(mm->root, key, /* create */ 0);
    if (node == NULL)
        return 0;

    while (i_ < node->fin_i) {
        if (node->values[i_] == value) {
            return 1;
        }
        else {
            i_++;
        }
    }

    return 0;
}


/* This helper function is used by mm_traverse() to traverse every pair within
 * the multimap.
 */
void mm_traverse_helper(multimap_node *node, void (*f)(int key, int value)) {
    int curr = 0;

    if (node == NULL)
        return;

    mm_traverse_helper(node->left_child, f);

    while (curr < node->fin_i) {
        f(node->key, node->values[curr]);
        curr++;
    }

    mm_traverse_helper(node->right_child, f);
}


/* Performs an in-order traversal of the multimap, passing each (key, value)
 * pair to the specified function.
 */
void mm_traverse(multimap *mm, void (*f)(int key, int value)) {
    mm_traverse_helper(mm->root, f);
}

