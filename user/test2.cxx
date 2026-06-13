#include <iostream>
#include <cassert>
#include <unistd.h>
#include <sys/wait.h>
#include "hw1_test.h"

// the test checks the fork() behavior of the paid/free status:
int main() {
    // parent becomes a paid user
    assert(set_is_paid(1) == 0);

    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Fork failed\n";
        return 1;
    }

    if (pid == 0) {
        // childs
        // edge case 1: did the child inherit exact same status of parent
        assert(get_is_paid() == 1);
        
        // edge case 2: changing the chilkd status must work
        assert(set_is_paid(0) == 0);
        assert(get_is_paid() == 0);
        
        return 0;
    } else {
        // parent
        int status;
        waitpid(pid, &status, 0);
        
        // ensure the child's assertions didn't fail
        assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
        
        // edge case 3: 
        // checking that parents status had not changed due to child modifications
        assert(get_is_paid() == 1);
    }

    std::cout << "test2 [fork inheritance & isolation] passed\n";
    return 0;
}