#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sys/types.h>
#include <unistd.h> 


struct Request { 
    pid_t pid; 
    int priority; 
    int paid; 
};

static bool runs_before(const Request &a, const Request &b) { 
    if (a.paid != b.paid) return a.paid > b.paid; 
    if (a.priority != b.priority) return a.priority > b.priority;
    return a.pid < b.pid; 
}

// some more math, more chaotically + integration test
int main() {
    std::vector<Request> batch = {
        {1005, 10, 0}, // free, low prio
        {1002, 90, 1}, // paid, high prio
        {1004, 90, 0}, // free, high prio
        {1001, 90, 1}, // paid, high prio, older (SHOULD BE FIRST)
        {1003, 50, 1}  // paid, med prio
    };

    std::sort(batch.begin(), batch.end(), runs_before);

    // The mathematically perfect order:
    assert(batch[0].pid == 1001); // paid, Prio 90, smaller PID
    assert(batch[1].pid == 1002); // paid, Prio 90, larger PID
    assert(batch[2].pid == 1003); // paid, Prio 50
    assert(batch[3].pid == 1004); // free, Prio 90
    assert(batch[4].pid == 1005); // free, Prio 10

    std::cout << "test5 [chaos and integration] passed!\n";
    return 0;
}