/* * dispatcher.cxx -- the user-space scheduler. 
 */ 
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

// Assuming hw1_test.h contains the wrappers for your custom system calls:
// void set_is_paid(int paid);
// int get_is_paid();
// int get_ai_token_priority();
#include "hw1_test.h" 

struct Request { 
        pid_t pid; 
        int   priority;   
        int   paid;       
}; 

/* * Strict weak ordering for the scheduler:
 * 1. Paid (1) runs before Free (0).
 * 2. Higher AI token priority runs first.
 * 3. Smaller PID runs first (tie-breaker).
 */ 
static bool runs_before(const Request &a, const Request &b) 
{ 
        if (a.paid != b.paid) {
            return a.paid > b.paid; 
        }
        if (a.priority != b.priority) {
            return a.priority > b.priority;
        }
        return a.pid < b.pid; 
} 

int main() 
{ 
        const int N = 4; 
        std::vector<Request> reports; 
        
        // pipe for children to send their Request structs to the parent
        int report_pipe[2];
        if (pipe(report_pipe) == -1) {
            perror("pipe failed");
            return 1;
        }

        std::cout << "[parent] Forking " << N << " request children...\n";

        for (int i = 0; i < N; ++i) {
            pid_t pid = fork();
            
            if (pid < 0) {
                perror("fork failed");
                return 1;
            }

            if (pid == 0) { 
                // child
                close(report_pipe[0]); // Close read end of the pipe

                // 1) simulate different combinations for testing
                int simulated_paid = (i % 2 == 0) ? 1 : 0; 

                set_is_paid(simulated_paid);

                // 2) gather data using the system calls you implemented
                Request req;
                req.pid = getpid();
                req.paid = get_is_paid();
                req.priority = get_ai_token_priority();

                // 3) report back to parent (atomic write since sizeof(Request) < PIPE_BUF)
                write(report_pipe[1], &req, sizeof(req));
                close(report_pipe[1]);

                // 4) block until the parent releases us
                std::cout << "[child " << getpid() << "] Stopping and waiting for release...\n";
                raise(SIGSTOP);

                // 5) next section runs ONLY after receiving SIGCONT
                std::cout << "[child " << getpid() << "] Executing job and exiting!\n";
                exit(0); 
            }
        }

        // parent
        close(report_pipe[1]); // Close write end of the pipe

        // collect all reports (Avoids deadlocks)
        for (int i = 0; i < N; ++i) {
            Request req;
            if (read(report_pipe[0], &req, sizeof(req)) == sizeof(req)) {
                reports.push_back(req);
            }
        }
        close(report_pipe[0]);

        // Important pphase: ensure all children have actually reached the STOPPED state.
        // if we send SIGCONT before they execute raise(SIGSTOP), it will be ignored,
        // and they will hang forever. WUNTRACED catches them entering the stopped state.
        for (const Request& r : reports) {
            int status;
            waitpid(r.pid, &status, WUNTRACED);
        }

        // sort the reports based on the scheduling rules
        std::sort(reports.begin(), reports.end(), runs_before); 

        std::cout << "\n--- Planned Run Order ---\n"; 
        for (const Request &r : reports) {
                std::cout << "  pid=" << r.pid 
                          << " priority=" << r.priority 
                          << " paid=" << r.paid << "\n"; 
        }
        std::cout << "-------------------------\n\n";

        // releasae children strictly in the sorted order
        for (const Request &r : reports) {
            kill(r.pid, SIGCONT);
            
            // wait for this specific child to finish execution before releasing the next one
            waitpid(r.pid, NULL, 0); 
        }

        std::cout << "[parent] All requests completed. Exiting.\n";
        return 0; 
}