#include <stdio.h>
#include "glue.h"


void report_result(const char *msg, int pass) {
    printf("%s:  %s\n", pass ? "PASS" : "ERROR", msg);
}


/* A simple program for exercising the scheduler lock to make sure it
 * works correctly.  Note that this program does not test the concurrency
 * behavior of the scheduler lock!
 */
int main() {
    int ret;

    /* Initially the scheduler lock should be 0, so it should be possible
     * to acquire it.
     */
    ret = __sthread_lock();
    report_result("Initial call to __sthread_lock() should acquire lock.",
                  ret != 0);

    /* Now that it is held, a second call should report failure to acquire
     * the lock.
     */
    ret = __sthread_lock();
    report_result("Second call to __sthread_lock() should not acquire lock.",
                  ret == 0);

    printf("Releasing lock.\n");
    __sthread_unlock();

    /* The lock should now be 0, so a third attempt to acquire the lock
     * should succeed.
     */
    ret = __sthread_lock();
    report_result("Third call to __sthread_lock() should acquire lock.",
                  ret != 0);

    /* Finally, a fourth attempt to acquire the lock should fail, since it is
     * currently held.
     */
    ret = __sthread_lock();
    report_result("Fourth call to __sthread_lock() should not acquire lock.",
                  ret == 0);

    return 0;
}

