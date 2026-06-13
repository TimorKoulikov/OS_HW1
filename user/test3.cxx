#include <iostream>
#include <cassert>
#include <errno.h>
#include "hw1_test.h"

// errno and bounds tests
int main() {
    // using an array of out-of-bounds inputs
    int bad_vals[] = {-1, 2, 7, 100, -999, 2000000000};
    
    for (int val : bad_vals) {
        errno = 0; // reseting errno before each test
    
        assert(set_is_paid(val) == -1); // the syscall must return -1
        assert(errno == EINVAL);
    }

    // Edge Case: Ensure failing didn't corrupt the underlying state
    assert(set_is_paid(1) == 0);
    assert(get_is_paid() == 1);

    std::cout << "test3 [strict bounds & errno] passed\n";
    return 0;
}