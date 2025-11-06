
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <csignal>
#include <chrono>
#include <thread>

int main() {
    pid_t pid1 = fork();

    if (pid1 < 0) {
        std::cerr << "Fork failed for process 1" << std::endl;
        exit(1);
    }

    // ------------------- PROCESS 1 -------------------
    if (pid1 == 0) {
        int counter = 0;
        int cycle = 0;

        std::cout << "Process 1 (PID " << getpid() << ") started." << std::endl;

        while (true) {
            if (counter % 3 == 0)
                std::cout << "Cycle number: " << cycle
                          << " – " << counter << " is a multiple of 3" << std::endl;
            else
                std::cout << "Cycle number: " << cycle << std::endl;

            counter++;
            cycle++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // slow display
        }

        exit(0); // not reached
    }

    // ------------------- PROCESS 2 -------------------
    pid_t pid2 = fork();
    if (pid2 < 0) {
        std::cerr << "Fork failed for process 2" << std::endl;
        exit(1);
    }

    if (pid2 == 0) {
        execl("./main", "process2", nullptr);
        // if execl fails
        std::cerr << "Failed to exec process2" << std::endl;
        exit(1);
    }

    // ------------------- PARENT -------------------
    std::cout << "Parent (PID " << getpid() << ") created two child processes." << std::endl;
    std::cout << "Use 'ps' to find their PIDs and 'kill PID' to stop them." << std::endl;

    // wait for children to terminate (normally you'd kill them manually)
     // Parent process

    //waitpid is a better version of wait() because it allows you to specify which child process to wait for. 
    //we will pass in child pid, address of status variable to store exit status, and options (0 means no options)


    //parent will wait for the pid2 (process2) to finish, when it does,
    //status will be updated with the exit status of process2
    //we are essentially pausing the parent process here until process2 finishes
    //and it will then kill the PCB of it and everything. If it ends, we will 
    //continue by terminating process1 as well.
    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);

    return 0;
}
