#ifndef EVAL_H
#define EVAL_H

#include "global.h"
#include "types.h"


void print_ref(Reference ref);

void eval_stmt(struct ParseStatement *stmt);


int foreach_global(void (*f)(const char *name, Reference ref));
void print_globals(void);


#endif /* EVAL_H */

