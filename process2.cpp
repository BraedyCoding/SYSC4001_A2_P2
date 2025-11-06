//////CODE FROM PART 2 and 3 ///////////////////////////

// #include <unistd.h>
// #include <iostream>
// #include <sys/types.h>
// #include <sys/wait.h>   // <-- needed for waitpid
// #include <cstdlib>      // for exit
// #include <chrono>
// #include <thread>       // for std::this_thread::sleep_for


// int main(void){


//     std::cout << "Child process is running" << std::endl;
//     // Child process 2
//     int counter1 = 0;
//     int cycle_counter = 0;

//      while (true) {

//         if(counter1 % 3 == 0) {
//             std::cout<<"Cycle Number "<<cycle_counter++<<" Process 2 Counter: "<<counter1--<<" - Multiple of 3!"<<std::endl;
//         }
//         else{   
//              std::cout << "Cycle number " << cycle_counter++ << " Process 2 Counter: " << counter1--<< std::endl;
//         }
        
//           std::this_thread::sleep_for(std::chrono::milliseconds(100)); // slow down output

//         if (counter1 == -500) {
//             exit(0); // Exit condition to end the process
//         }
//     }
// }



///Code for #4-5 ///////////////////////////
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <chrono>
#include <thread>

// Wait (P)
void sem_wait_custom(int semid) {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

// Signal (V)
void sem_signal_custom(int semid) {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}

int main() {
    key_t sem_key = 65;
    key_t shm_key = 1234;

    // Attach to semaphore
    int semid = semget(sem_key, 1, 0666);
    if (semid < 0) {
        std::cerr << "Child2: Failed to access semaphore\n";
        exit(1);
    }

    // Attach to shared memory
    int shmid = shmget(shm_key, 2 * sizeof(int), 0666);
    if (shmid < 0) {
        std::cerr << "Child2: Failed to access shared memory\n";
        exit(1);
    }

    int* shared_mem = (int*)shmat(shmid, nullptr, 0);
    if (shared_mem == (void*)-1) {
        std::cerr << "Child2: Failed to attach shared memory\n";
        exit(1);
    }

    std::cout << "Child2 waiting for counter > 100...\n";
    while (shared_mem[1] <= 100)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    int cycle_counter = 0;
    while (shared_mem[1] < 500) {
        sem_wait_custom(semid);
        shared_mem[1]--;
        if (shared_mem[1] % shared_mem[0] == 0)
            std::cout << "Cycle " << cycle_counter++ << " | Child2 Counter: "
                      << shared_mem[1] << " (multiple of " << shared_mem[0] << ")\n";
        else
            std::cout << "Cycle " << cycle_counter++ << " | Child2 Counter: "
                      << shared_mem[1] << "\n";
        sem_signal_custom(semid);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    shmdt(shared_mem);
    return 0;
}
