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
    int execution_pipe[2]; 
    
    pipe(report_pipe);
    pipe(execution_pipe);

    // fork 3 FREE children to test the dynamic AI token priority */
    for (int i = 0; i < 3; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                    //child
                    close(report_pipe[0]);
                    close(execution_pipe[0]);

                    set_is_paid(0);

                    Request req;
                    req.pid = getpid();
                    req.priority = get_ai_token_priority();
                    req.paid = get_is_paid();

                    write(report_pipe[1], &req, sizeof(req));
                    close(report_pipe[1]);

                    raise(SIGSTOP);

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

    for (const Request& r : reports) {
            int status;
            waitpid(r.pid, &status, WUNTRACED);
    }

    std::sort(reports.begin(), reports.end(), runs_before);

    // wake them up
    for (const Request &r : reports) {
            kill(r.pid, SIGCONT);
            waitpid(r.pid, NULL, 0); 
    }

    // verify they executed in the exact sorted order
    for (size_t i = 0; i < reports.size(); i++) {
            pid_t executed_pid;
            read(execution_pipe[0], &executed_pid, sizeof(pid_t));
            assert(executed_pid == reports[i].pid);
    }
    close(execution_pipe[0]);

    std::cout << "test7 [OS Integration: priority Scheduler] passed!\n";
    return 0;
}