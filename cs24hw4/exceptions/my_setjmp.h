#ifndef MY_SETJMP
#define MY_SETJMP

#include <stdint.h>

/*
 * Change to #define to enable the use of your own setjmp()/longjmp()
 * implementation instead of the standard one, or undefine it if you
 * want to stick with the standard one!
 */
#undef ENABLE_MY_SETJMP


#ifdef ENABLE_MY_SETJMP

/*
 * We are daring and we're going to try out our own implementation of
 * setjmp() and longjmp()!  Watch out stack, here we come!
 */

/* TODO:  Set this to the proper size for your jump-buffer. */
#define MY_JB_LEN 0
typedef uint64_t my_jmp_buf[MY_JB_LEN];

int my_setjmp(my_jmp_buf buf);
void my_longjmp(my_jmp_buf buf, int ret);

/*
 * Just in case these symbols were defined elsewhere, blow them away
 * and redefine them for ourselves.
 */

#undef jmp_buf
#undef setjmp
#undef longjmp

#define jmp_buf my_jmp_buf
#define setjmp my_setjmp
#define longjmp my_longjmp

#else

/*
 * We are going to stick with the standard implementation of setjmp()
 * and longjmp() for now...
 */
#include <setjmp.h>

#endif /* ENABLE_MY_SETJMP */

#endif /* MY_SETJMP */

