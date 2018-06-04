#include <stdio.h>
#include "sthread.h"

/* Test an incrementing loop again, this time ensuring that
 * the arg stays less than that in the previous
 */

static void inc_loop(void *arg) {
	for (int i = 1; i <= *((int *) arg); i++) {
		printf("Argument = %d, Loop Count = %d\n", *((int *) arg), i);
		sthread_yield();
	}
}


int main(int argc, char const *argv[])
{
	int first = 10;
	int second = 8;
	int third = 12;
	int fourth = 2;

	sthread_create(inc_loop, &first);
	sthread_create(inc_loop, &second);
	sthread_create(inc_loop, &third);
	sthread_create(inc_loop, &fourth);
	sthread_start();
	return 0;
}