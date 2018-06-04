#include "my_setjmp.h"
#include <stdio.h>

static jmp_buf buf;

/* Ensures that longjmp returns an acceptable value when passed a buffer and a
 * return value of 0. It should return 1 if the passed return value is 0. This
 * function will return 1 if this is the case, and -1 otherwise.
 */

int check_zero_ret() {
	jmp_buf cur;

	switch(setjmp(cur)) {
		case 0:
			longjmp(cur, 0);

		case 1:
			return 1;

		default:
			return -1;
	}
}

/* Checks that longjmp returns the passed value when passed a buffer and a
 * return value != 0. It will return n if the value is acceptable, and -1
 * otherwise. (return -1 is simply a placeholder to ensure we are not getting
 * a very weird error).
 */

int check_other_ret(int n) {
	jmp_buf cur;
	int ret = setjmp(cur);

	if (ret == 0) {
		longjmp(cur, n);
	}
	else if (ret == n) {
		return n;
	}
	else {
		return 0;
	}
	return -1;
}


void jump_init();

/* Checks to ensure that longjmp can cross funtion calls. Will reuturn 1 if
 * this is the case, 0 otherwise (again, return -1 is a placeholder).
 */

int check_jumping() {
	if (setjmp(buf) == 0) {
		jump_init();
		return 0;
	}
	else {
		return 1;
	}
	return -1;
}

void jump_init() {
	longjmp(buf, 10);
}

/* The most complex test; ensures that neither setjmp nor longjmp corrupt the
 * stack. We test this by running ints through the stack and comparing before
 * and after.
 */

int check_corruption() {
	int prev1 = 1;
	int prev2 = 2;
	int prev3 = 3;
	int proc1 = 11;
	int proc2 = 12;
	int proc3 = 13;
	jmp_buf cur;

	if (setjmp(cur) == 0) {
		longjmp(cur, 1);
	}
	else {
		if (prev1 == 1 && prev2 == 2 && prev3 == 3 &&
			proc1 == 11 && proc2 == 12 && proc3 == 13){
			return 1;
		}
		else {
			return 0;
		}
	}
	return -1;
}

int main()
{
	// Test count
	int passed = 0;

	// Check with return value of 0
	int check_first = check_zero_ret();

	if (check_first == 1) {
		printf("%s\n\n", "TEST 1: PASSED");
		passed++;
	}
	else {
		printf("%s\n\n", "TEST 1: FAILED");
	}

	// Check with nonzero return value
	int n_val = 15;
	int check_second = check_other_ret(n_val);

	if (check_second = n_val) {
		printf("%s\n\n", "TEST 2A: PASSED");
		passed++;
	}
	else {
		printf("%s\n\n", "TEST 2A: FAILED");
	}

	// Must check the second test for negative values as well.

	n_val = -15;
	check_second = check_other_ret(n_val);

	if (check_second = n_val) {
		printf("%s\n\n", "TEST 2B: PASSED");
		passed++;
	}
	else {
		printf("%s\n\n", "TEST 2B: FAILED");
	}

	// Check jumping of functions
	int check_third = check_jumping();

	if (check_third > 0) {
		printf("%s\n\n", "TEST 3: PASSED");
		passed++;
	}
	else {
		printf("%s\n\n", "TEST 3: FAILED");
	}

	// Finally, check to ensure that no stack corruption has occured
	int check_fourth = check_corruption();

	if (check_fourth > 0) {
		printf("%s\n\n", "TEST 4: PASSED");
		passed++;
	}
	else {
		printf("%s\n\n", "TEST 4: FAILED");
	}

	if (passed == 5) {
		printf("%s\n", "ALL TESTS PASSED!");
	}
	else {
		printf("%d%s\n", passed, " TESTS PASSED");
	}
}