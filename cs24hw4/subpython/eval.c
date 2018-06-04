#include <assert.h>
#include <string.h>

#include "global.h"
#include "eval.h"
#include "alloc.h"

/* Global variable information. */

#define MAX_DEPTH 4

struct GlobalVariable {
    char *name;
    Reference ref;
} *global_vars = NULL;
int num_vars = 0;
int max_vars = 0;


//// LOCAL FUNCTION DECLARATIONS ////

// Helpers
Reference *get_global_variable(char *name, bool create);
void delete_global_variable(char *name);

void print_ref_ext(Reference ref, bool newline, int depth);

Reference eval_expr(struct ParseExpression *expr);
Reference *eval_expr_lval(struct ParseExpression *expr);

float eval_expect_float(struct ParseExpression *expr);

Reference make_reference_float(float f);
Reference make_reference_string(char *c);
Reference make_reference_list_node(Reference next, Reference value);
Reference make_reference_dict_node(Reference next, Reference key, Reference value);

bool key_equals(Reference a, Reference b);
Reference key_clone(Reference ref);


//// HELPER FUNCTIONS ////


ListValue * to_list_value(Value *v) {
    ListValue *lv = NULL;
    if (v) {
        assert(v->type == VAL_LIST_NODE);
        lv = (ListValue *) v;
    }
    return lv;
}

ListValue * deref_to_list_value(Reference ref) {
    return to_list_value(deref(ref));
}


DictValue * to_dict_value(Value *v) {
    DictValue *dv = NULL;
    if (v) {
        assert(v->type == VAL_DICT_NODE);
        dv = (DictValue *) v;
    }
    return dv;
}

DictValue * deref_to_dict_value(Reference ref) {
    return to_dict_value(deref(ref));
}


/*!
 * Returns the element of the list at index idx, or reports an error if the
 * list doesn't have an element at that index.
 */
ListValue * get_list_elem(Reference ref, int idx) {
    /* The first node in a list is always a dummy value, so we can ignore it.
     * (Also, verify that it actually doesn't hold anything.)
     */
    ListValue *elem = deref_to_list_value(ref);
    assert(elem->list_node.value == NULL_REF);

    /* Skip past the dummy element.  Also, handle the case where the list
     * is empty.
     */
    elem = deref_to_list_value(elem->list_node.next);
    if (elem == NULL) {
        error(-1, "%s", "Index out of bounds: %d out of %d.", idx, 0);
    }

    /* Find the `idx`th entry in the list. */
    for (int i = 0; i < idx; i++) {
        if (elem == NULL) {
            error(-1, "Index out of bounds: %d out of %d.", idx, i);
        }

        ref = elem->list_node.next;
        elem = deref_to_list_value(ref);
    }

    if (elem == NULL) {
        error(-1, "Index out of bounds: %d out of %d.", idx, idx);
    }

    return elem;

}


DictValue * get_dict_entry(Reference ref, Reference key, bool create) {

    /* The first node in a dictionary is always a dummy value, so we can
     * ignore it.  (Also, verify that it actually doesn't hold anything.)
     */
    DictValue *start = deref_to_dict_value(ref);
    DictValue *entry = start;
    DictValue *prev;
    assert(start->dict_node.key == NULL_REF);
    assert(start->dict_node.value == NULL_REF);

    /* Move past the first entry. */
    entry = deref_to_dict_value(entry->dict_node.next);
    prev = start;

    /* Iterate until we find our key, or until we reach the end of the
     * dictionary's entries. */
    while (entry != NULL) {
        if (key_equals(entry->dict_node.key, key)) {
            break;
        }

        prev = entry;
        entry = deref_to_dict_value(entry->dict_node.next);
    }

    /* If we got NULL, then that means our key is missing. */
    if (entry == NULL) {
        if (!create) {
            /* The caller wants us to report an error. */
            error(-1, "%s", "Key cannot be found!");
        }
        else {
            /* The caller wants us to create a new entry.  Tack it onto the
             * end of the dictionary.  Set the key, but we don't know what
             * value it should have yet.
             */

            Reference entry_ref =
                make_reference_dict_node(NULL_REF, key, NULL_REF);
            entry = (DictValue *) deref(entry_ref);

            assert(prev != NULL);
            assert(prev->dict_node.next = NULL_REF);
            prev->dict_node.next = entry_ref;
        }
    }

    return entry;
}


//// CODE ////

void print_list(Reference ref, int depth) {
    bool first = true;

    /* The first element is a dummy element.  Skip it. */
    ListValue *lv = deref_to_list_value(ref);
    assert(lv->list_node.value == NULL_REF);
    lv = deref_to_list_value(lv->list_node.next);

    while (lv != NULL) {
        if (first) {
            first = false;
        } else {
            fprintf(stdout, ", ");
        }

        if (depth != 0) {
            print_ref_ext(lv->list_node.value, false, depth - 1);
        } else {
            fprintf(stdout, "...");
        }

        lv = deref_to_list_value(lv->list_node.next);
    }
}

void print_dict(Reference ref, int depth) {
    bool first = true;

    /* The first entry is a dummy entry.  Skip it. */
    DictValue *dv = deref_to_dict_value(ref);
    assert(dv->dict_node.key == NULL_REF);
    assert(dv->dict_node.value == NULL_REF);
    dv = deref_to_dict_value(dv->dict_node.next);

    while (dv != NULL) {
        if (first) {
            first = false;
        } else {
            fprintf(stdout, ", ");
        }

        /* depth irrelevant for keys */
        print_ref_ext(dv->dict_node.key, false, 0);

        fprintf(stdout, ": ");

        if (depth != 0) {
            print_ref_ext(dv->dict_node.value, false, depth - 1);
        } else {
            fprintf(stdout, "...");
        }

	    dv = deref_to_dict_value(dv->dict_node.next);
    }
}

void print_ref(Reference ref) {
    print_ref_ext(ref, true, MAX_DEPTH);
}

void print_ref_ext(Reference ref, bool newline, int depth) {
    Value *v = deref(ref);
    switch (v->type) {
        case VAL_FLOAT:
            fprintf(stdout, "%g", ((FloatValue *) v)->float_value);
            break;

        case VAL_STRING:
            fprintf(stdout, "\"%s\"", ((StringValue *) v)->string_value);
            break;

        case VAL_LIST_NODE:
            fprintf(stdout, "[");
            print_list(ref, depth);
            fprintf(stdout, "]");
            break;

        case VAL_DICT_NODE:
            fprintf(stdout, "{");
            print_dict(ref, depth);
            fprintf(stdout, "}");
            break;

        default:
            fprintf(stdout, "Unrecognized value type\n");
            break;
    }

    if (newline) {
        fprintf(stdout, "\n");
    }
}


//// EVALUATION IMPLEMENTATION ////


void eval_stmt(ParseStatement *stmt) {
    Reference eval_ref;

    switch (stmt->type) {
        case STMT_DEL:
            delete_global_variable(stmt->identifier);
            break;

        case STMT_EXPR:
            eval_ref = eval_expr(stmt->expr);
            if (stmt->expr->type != EXPR_ASSIGN) {
                print_ref_ext(eval_ref, true, MAX_DEPTH);
            }
            break;

        case STMT_GC:
            collect_garbage();
            break;

        case STMT_QUIT:
            printf("Quitting, goodbye.\n");
            exit(0);
            break;
    }
}

Reference eval_expr(ParseExpression *expr) {
    Reference lhs, rhs;

    switch (expr->type) {
        case EXPR_SUBSCRIPT:
            lhs = eval_expr(expr->lhs);
            Value *lhs_value = deref(lhs);

            if (lhs_value->type == VAL_LIST_NODE) {
                /* If we have a list, then floor the float to make an index.
                 * (it's the best we can do... without reintroducing ints.) */
                int idx = (int) eval_expect_float(expr->rhs);
                ListValue *elem = get_list_elem(lhs, idx);
                return elem->list_node.value;
            } else if (lhs_value->type == VAL_DICT_NODE) {
                /* If we have a dict, then evaluate our rhs key.  */
                rhs = eval_expr(expr->rhs);

                /* The first node in a dictionary is always a dummy value, so
                 * we can ignore it.
                 */
                DictValue *lhs_dict =
                    get_dict_entry(lhs, rhs, /* create */ false);
                return lhs_dict->dict_node.value;
            } else {
                error(-1, "%s", "Can only subscript lists and dictionaries.");
            }
        case EXPR_NEGATE: {
            float float_val = eval_expect_float(expr->lhs);
            return make_reference_float(-float_val);
        }
        case EXPR_IDENT:
            /* We dereference, because get_global_variable returns a
             * Reference*.
             */
            return *get_global_variable(expr->string, false);

        case EXPR_STRING:
            return make_reference_string(expr->string);

        case EXPR_FLOAT:
            return make_reference_float(expr->float_value);

        case EXPR_LIST: {
            /* Construct a new list by reversing the parse list, which was the
             * the reversed version of the parsed list = an in-order list! */
            ParseListNode *parse_list = expr->list;
            Reference eval_list_node = NULL_REF;

            while (parse_list != NULL) {
                eval_list_node =
                    make_reference_list_node(eval_list_node,
                                             eval_expr(parse_list->expr));
                parse_list = parse_list->next;
            }

            /* Finally, lists start with a dummy element, so tack one onto
             * the front.
             */
            eval_list_node = make_reference_list_node(eval_list_node, NULL_REF);

            return eval_list_node;
        }

        case EXPR_DICT: {
            /* Similar to list code. Almost identical, but s/List/Dict, and
             * there are both keys and values... */
            ParseDictNode *parse_dict = expr->dict;
            Reference eval_dict_node = NULL_REF;

            while (parse_dict != NULL) {
                eval_dict_node =
                    make_reference_dict_node(eval_dict_node,
                                             eval_expr(parse_dict->key),
                                             eval_expr(parse_dict->value));
                parse_dict = parse_dict->next;
            }

            /* Finally, dictionaries start with a dummy entry, so tack one
             * onto the front. */
            eval_dict_node =
                make_reference_dict_node(eval_dict_node, NULL_REF, NULL_REF);

            return eval_dict_node;
        }

        case EXPR_ASSIGN: {
            /* eval_expr_lval returns a Reference*, and we set it to the
             * rhs ref.
             */
            rhs = eval_expr(expr->rhs);
            Reference *lval = eval_expr_lval(expr->lhs);
            *lval = rhs;
            return *lval;
        }

        case EXPR_ADD: {
            float lhs_val = eval_expect_float(expr->lhs);
            float rhs_val = eval_expect_float(expr->rhs);
            return make_reference_float(lhs_val + rhs_val);
        }

        case EXPR_SUB: {
            float lhs_val = eval_expect_float(expr->lhs);
            float rhs_val = eval_expect_float(expr->rhs);
            return make_reference_float(lhs_val - rhs_val);
        }

        case EXPR_MULT: {
            float lhs_val = eval_expect_float(expr->lhs);
            float rhs_val = eval_expect_float(expr->rhs);
            return make_reference_float(lhs_val * rhs_val);
        }

        case EXPR_DIV: {
            float lhs_val = eval_expect_float(expr->lhs);
            float rhs_val = eval_expect_float(expr->rhs);
            return make_reference_float(lhs_val / rhs_val);
        }

        default:
            UNREACHABLE();
    }
}


/*!
 * Evaluates an expression that can be the target of an assignment.  This is why
 * the method returns a pointer to a Reference - so that we can change the
 * Reference itself to refer to something else.
 */
Reference * eval_expr_lval(ParseExpression *expr) {
    Reference lhs, rhs;
    Value *lhs_value;

    switch (expr->type) {
        case EXPR_SUBSCRIPT:
            lhs = eval_expr(expr->lhs);
            lhs_value = deref(lhs);

            if (lhs_value->type == VAL_LIST_NODE) {
                /* If we have a list, then floor the float to make an index.
                 * (it's the best we can do... without introducing ints.) */
                int idx = (int) eval_expect_float(expr->rhs);
                ListValue *elem = get_list_elem(lhs, idx);
                return &(elem->list_node.value);
            } else if (lhs_value->type == VAL_DICT_NODE) {
                /* If we have a dict, then evaluate our rhs key.  */
                rhs = eval_expr(expr->rhs);

                /* Look up the entry with this key; if there is none, create
                 * one.
                 */
            	DictValue *lhs_dict =
                    get_dict_entry(lhs, rhs, /* create */ true);
                return &(lhs_dict->dict_node.value);
            } else {
                error(-1, "%s", "Can only subscript lists and dictionaries.");
            }

        case EXPR_IDENT:
            return get_global_variable(expr->string, true);
            break;

        case EXPR_ASSIGN: {
            Reference *lval = eval_expr_lval(expr->lhs);
            rhs = eval_expr(expr->rhs);
            *lval = rhs;
            return lval;
        }

        default:
            UNREACHABLE();
    }
}

/*! Evaluate and expect a float, erroring if it's not a float, then returning
    that float... */
float eval_expect_float(ParseExpression *expr) {
    Reference ref = eval_expr(expr);
    Value *v = deref(ref);
    if (v->type != VAL_FLOAT) {
        error(-1, "%s", "Expected numerical (float) value.");
    }

    return ((FloatValue *) v)->float_value;
}

/*! Tries to retrieve a global variable's reference, creating it if `create`
    is true. */
Reference *get_global_variable(char *name, bool create) {
    for (int i = 0; i < num_vars; i++) {
        if (global_vars[i].name != NULL &&
            strcmp(name, global_vars[i].name) == 0) {
            return &global_vars[i].ref;
        }
    }

    if (create) {
        if (global_vars == NULL) {
            /* If our global vars array is NULL, let's make a new one. */
            global_vars = calloc(sizeof(struct GlobalVariable), INITIAL_SIZE);
            max_vars = INITIAL_SIZE;
        } else if (num_vars == max_vars) {
            /* Otherwise, double its size (the JVM internal source said this
             * was a good resizing semantic, don't sue me!), and zero it out. */
            max_vars *= 2;
            global_vars = realloc(global_vars,
                                  sizeof(struct GlobalVariable) * max_vars);
            // Zero so our loop searching for a new spot doesn't fail.
            memset(global_vars + num_vars, 0,
                   sizeof(struct GlobalVariable) * num_vars);
        }

        if (global_vars == NULL) {
            error(-1, "%s", "Allocation failed!");
        }

        /* Search for a new variable. */
        for (int i = 0; i < max_vars; i++) {
            if (global_vars[i].name == NULL) {
                num_vars++;
                global_vars[i].name = strndup(name, strlen(name));
                global_vars[i].ref = NULL_REF;
                return &global_vars[i].ref;
            }
        }

        /* We made space for at least 1 new var up there,
         * so this can't happen..! I swear! */
        UNREACHABLE();
    } else {
        error(-1, "Could not retrieve variable `%s`", name);
    }
}

/*! Delete the global variable with name `name`. Error if no such variable
    exists. */
void delete_global_variable(char *name) {
    int i;
    for (i = 0; i < num_vars; i++) {
        if (strcmp(name, global_vars[i].name) == 0) {
            break;
        }
    }

    if (i < num_vars) {
        // Found the variable.  Remove it by sliding the whole array down.

        free(global_vars[i].name);

        // REVIEW:  Would be faster to do this with a memmove(), but whatever
        for (int j = i; j < num_vars - 1; j++) {
            global_vars[j].name = global_vars[j + 1].name;
            global_vars[j].ref = global_vars[j + 1].ref;
        }

        num_vars--;
        global_vars[num_vars].name = NULL;
        global_vars[num_vars].ref = NULL_REF;

        // TODO resize array if too small. We'd need to compact it too.
    } else {
        error(-1, "Could not delete variable `%s`", name);
    }
}

/*!
 * Invokes a function for each global in the global environment.  Returns the
 * number of globals found.
 */
int foreach_global(void (*f)(const char *name, Reference Ref)) {
    for (int i = 0; i < num_vars; i++) {
        f(global_vars[i].name, global_vars[i].ref);
    }
    return num_vars;
}

void print_global_helper(const char *name, Reference ref) {
    fprintf(stdout, "%s = ref %d; value ", name, ref);
    print_ref_ext(ref, true, MAX_DEPTH);
}

void print_globals(void) {
    // Just so we can make the text reflect the number of globals.
    if (num_vars == 0)
        fprintf(stdout, "0 Globals\n");
    else if (num_vars == 1)
        fprintf(stdout, "1 Global:\n");
    else
        fprintf(stdout, "%d Globals:\n", num_vars);

    foreach_global(print_global_helper);
}

/*! Returns true if two keys are equal. Only works on strings and floats. */
bool key_equals(Reference a, Reference b) {
    Value *va = deref(a);
    Value *vb = deref(b);

    assert(va != NULL);
    assert(vb != NULL);

    if (va->type != vb->type) {
        return false;
    }

    switch (va->type) {
        case VAL_FLOAT:
            return ((FloatValue *) va)->float_value ==
                   ((FloatValue *) vb)->float_value;

        case VAL_STRING:
            return strcmp(((StringValue *) va)->string_value,
                          ((StringValue *) vb)->string_value) == 0;

        case VAL_LIST_NODE:
        case VAL_DICT_NODE:
            error(-1, "%s", "Dict and List types are not valid key types.");

        default:
            UNREACHABLE();
    }
}

/*! ListNode allocation helper. */
Reference make_reference_list_node(Reference next, Reference value) {
    ListValue *lv = (ListValue *) alloc(VAL_LIST_NODE, /* ignored */ 0);
    lv->list_node.value = value;
    lv->list_node.next = next;
    return lv->ref;
}

/*! DictNode allocation helper. */
Reference make_reference_dict_node(Reference next, Reference key, Reference value) {
    DictValue *dv = (DictValue *) alloc(VAL_DICT_NODE, /* ignored */ 0);
    dv->dict_node.key = key;
    dv->dict_node.value = value;
    dv->dict_node.next = next;
    return dv->ref;
}

/*! Assigns a float to a new reference in the ref_table. */
Reference make_reference_float(float f) {
    FloatValue *fv = (FloatValue *) alloc(VAL_FLOAT, /* ignored */ 0);
    fv->float_value = f;
    return fv->ref;
}

/*! Assigns a string to a new reference in the ref_table. */
Reference make_reference_string(char *c) {
    StringValue *sv = (StringValue *) alloc(VAL_STRING, strlen(c) + 1);
    strcpy(sv->string_value, c);
    return sv->ref;
}

/*! Clones a key for a dictionary, so keys aren't accidentally bound to each
    other. */
Reference key_clone(Reference ref) {
    // Clone any non-deep type, float and string (that's it...) which are the
    // current key types, as well...
    Value *v = deref(ref);
    switch (v->type) {
        case VAL_FLOAT:
            return make_reference_float(((FloatValue *) v)->float_value);
        case VAL_STRING:
            return make_reference_string(((StringValue *) v)->string_value);
        default:
            error(-1, "%s", "Only numerical (floats) and string types are "
                            "supported as keys!");
            return 0;
    }
}

