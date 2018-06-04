#include <stdio.h>
#include "sthread.h"


/* Test an incrementing loop */

static void inc_loop(void *arg) {
	while(1) {
		*((int *) arg) += 1;
		printf("%s%d\n", "Value of variable = ", *((int *) arg));
		sthread_yield();
	}
}

/* Test a decrementing loop */

static void dec_loop(void *arg) {
	while(1) {
		*((int *) arg) -= 1;
		printf("%s%d\n", "Value of variable = ", *((int *) arg));
		sthread_yield();
	}
}

int main(int argc, char const *argv[])
{
	int start = 0; /* starting value of variable */
	printf("%s%d\n", "Starting value of variable = ", start);

	sthread_create(inc_loop, &start);
	sthread_create(dec_loop, &start);
	sthread_start();

	return 0;
}