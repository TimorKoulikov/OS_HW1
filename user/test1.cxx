/*
 * test1.cxx -- a minimal sanity test. EXTEND THIS / ADD YOUR OWN TESTS.
 * Part of your grade for the user-space part is the quality of YOUR tests.
 *
 * Build & run:   g++ test1.cxx -o test1 && ./test1
 */
#include <iostream>
#include <cassert>
#include "hw1_test.h"

int main()
{
        /* set_is_paid / get_is_paid round-trip */
        assert(set_is_paid(1) == 0);
        assert(get_is_paid() == 1);
        assert(set_is_paid(0) == 0);
        assert(get_is_paid() == 0);

        /* invalid input must fail with -1 and errno == EINVAL */
        /* TODO: check set_is_paid(7) and set_is_paid(-1) */

        /* priority is (pid + parent_pid) % 100 and is in [0,99] */
        int p = get_ai_token_priority();
        std::cout << "my priority = " << p << std::endl;
        assert(p >= 0 && p < 100);

        std::cout << "test1 passed" << std::endl;
        return 0;
}
