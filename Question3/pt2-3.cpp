


//Background:
// We want to make sure that the child processes are terminated 
// before the parent process finishes. This is because if a child 
// terminates **while the parent is still alive** and the parent 
// does not call waitpid(), the child becomes a zombie process. 
// The PCB of the child remains in memory so the parent can access 
// its exit status, which uses up RAM.  
// If the parent dies before the child finishes, the kernel will 
// adopt the child and automatically clean up its PCB when it exits, 
// so no zombie is left.  
// To properly handle child termination and retrieve their exit status, 
// we use waitpid(), passing in the child’s PID, the address of a status 
// variable, and options (0 means no special options).







#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>   // <-- needed for waitpid
#include <cstdlib>      // for exit
#include <chrono>
#include <thread>       // for std::this_thread::sleep_for



//Background:
// We want to make sure that the child processes are terminated 
// before the parent process finishes. This is because if a child 
// terminates **while the parent is still alive** and the parent 
// does not call waitpid(), the child becomes a zombie process. 
// The PCB of the child remains in memory so the parent can access 
// its exit status, which uses up RAM.  
// If the parent dies before the child finishes, the kernel will 
// adopt the child and automatically clean up its PCB when it exits, 
// so no zombie is left.  
// To properly handle child termination and retrieve their exit status, 
// we use waitpid(), passing in the child’s PID, the address of a status 
// variable, and options (0 means no special options).

int main(void) {
    pid_t pid1 = fork(); // Create Process 1
    pid_t pid2;

    if (pid1 < 0) {
        std::cerr << "Fork failed for Process 1" << std::endl;
        exit(1);
    }

    // ---------------- PROCESS 1 ----------------
    else if (pid1 == 0) {
        int cycle_counter1 = 0;
        int counter1 = 0;

        std::cout << "Process 1 (PID " << getpid() << ") started.\n" << std::flush;

        while (true) {
        
        std::cout << "Cycle number " << cycle_counter1
                  << " | Process 1 Counter: " << counter1;

        // Display special message for multiples of 3
        if (counter1 % 3 == 0)
            std::cout << " - Multiple of 3!";

        std::cout << std::endl << std::flush;

        cycle_counter1++; // increment cycle counter
        counter1++; // always increment
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // adjust speed
    }

    exit(0); // never reached
}
    // ---------------- PARENT ----------------
    else {
        std::cout << "Parent process (PID " << getpid() << ") is running.\n";

        // Create Process 2
        pid2 = fork();

        if (pid2 < 0) {
            std::cerr << "Fork failed for Process 2" << std::endl;
            exit(1);
        }

        // ---------------- PROCESS 2 ----------------
        else if (pid2 == 0) {
            execl("./process2", "process2", nullptr);
            std::cerr << "Error: exec failed for Process 2" << std::endl;
            exit(1);
        }

        // ---------------- PARENT WAITS ----------------
        else {
            int status;
            // Wait for Process 2 to finish (reach counter < -500)
            wait(&status);

            std::cout << "\nProcess 2 finished (reached -500). "
                         "Terminating Process 1...\n";
            kill(pid1, SIGTERM);  // Terminate Process 1
        }
    }

    return 0;
}
