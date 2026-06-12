#ifndef HW1_TEST_H
#define HW1_TEST_H

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

/*
 * IMPORTANT: these numbers MUST be identical to the numbers you reserved
 * in arch/x86/entry/syscalls/syscall_64.tbl on your kernel. If you used
 * different numbers, change them here.
 */
#define __NR_hello                  333
#define __NR_set_is_paid            334
#define __NR_get_is_paid            335
#define __NR_get_ai_token_priority  336

/* Thin user-space wrappers around the raw syscall() interface. */
static inline long hello(void) {
        return syscall(__NR_hello);
}
static inline int set_is_paid(int is_paid) {
        return (int) syscall(__NR_set_is_paid, is_paid);
}
static inline int get_is_paid(void) {
        return (int) syscall(__NR_get_is_paid);
}
static inline int get_ai_token_priority(void) {
        return (int) syscall(__NR_get_ai_token_priority);
}

#endif /* HW1_TEST_H */
