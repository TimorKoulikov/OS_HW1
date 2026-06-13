#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <cassert>
#include <signal.h>
#include "hw1_test.h"

struct Request {
    pid_t pid;
    int priority;
    int paid;
};

static bool runs_before(const Request &a, const Request &b)
{
    if (a.paid != b.paid) return a.paid > b.paid;
    if (a.priority != b.priority) return a.priority > b.priority;
    return a.pid < b.pid;
}

int main(){
    int report_pipe[2];
    int execution_pipe[2]; /* Used to prove the exact order children wake up */
    
    assert(pipe(report_pipe) == 0);
    assert(pipe(execution_pipe) == 0);

    /* We will fork 2 children.
        * Child 0: Free (0)
        * Child 1: Paid (1)
        * Even though Child 0 is older, Child 1 MUST be scheduled first.
        */
    pid_t pids[2];
    int paid_status[2] = {0, 1};

    for (int i = 0; i < 2; i++) {
            pids[i] = fork();
            if (pids[i] == 0) {
                    //child
                    close(report_pipe[0]);
                    close(execution_pipe[0]);

                    assert(set_is_paid(paid_status[i]) == 0);

                    Request req;
                    req.pid = getpid();
                    req.priority = get_ai_token_priority();
                    req.paid = get_is_paid();

                    write(report_pipe[1], &req, sizeof(req));
                    close(report_pipe[1]);

                    // block and wait to be scheduled by the parent
                    raise(SIGSTOP);

                    // we have been awakened! Write our PID to prove we are running.
                    write(execution_pipe[1], &req.pid, sizeof(pid_t));
                    close(execution_pipe[1]);
                    return 0;
            }
    }

    //parent
    close(report_pipe[1]);
    close(execution_pipe[1]);

    std::vector<Request> reports;
    Request req;
    while (read(report_pipe[0], &req, sizeof(req)) == sizeof(req)) {
            reports.push_back(req);
    }
    close(report_pipe[0]);

    // Ensure all children are physically stopped before scheduling
    for (const Request& r : reports) {
            int status;
            waitpid(r.pid, &status, WUNTRACED);
    }

    // sort using the scheduler logic
    std::sort(reports.begin(), reports.end(), runs_before);

    // assert our sorting math worked: The paid child MUST be at index 0
    assert(reports[0].paid == 1);

    // wake them up in the sorted order
    for (const Request &r : reports) {
            kill(r.pid, SIGCONT);
            waitpid(r.pid, NULL, 0); /* Wait for it to finish completely */
    }

    // read the execution pipe. The first PID written must be the paid child's PID.
    pid_t executed_first, executed_second;
    read(execution_pipe[0], &executed_first, sizeof(pid_t));
    read(execution_pipe[0], &executed_second, sizeof(pid_t));
    close(execution_pipe[0]);

    // OS validation
    assert(executed_first == reports[0].pid);
    assert(executed_second == reports[1].pid);

    std::cout << "test6 [OS integration: paid scheduler] passed!\n";
    return 0;
}