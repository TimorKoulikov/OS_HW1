#include <iostream>
#include <cassert>
#include <unistd.h>
#include "hw1_test.h"

// a bit similar to test1, but checks the dynamic math of the priority calculation
int main() {
    // kernel's priority
    int kernel_priority = get_ai_token_priority();
    
    // user-visible priority is (pid + parent_pid) % 100
    int expected_priority = (getpid() + getppid()) % 100;
    
    //must match perfectly
    assert(kernel_priority == expected_priority);
    
    // Priority must physically be bounded between 0 and 99
    assert(kernel_priority >= 0 && kernel_priority <= 99);

    std::cout << "test4 [dynamic priority math] passed, priority = " << kernel_priority << "\n";
    return 0;
}