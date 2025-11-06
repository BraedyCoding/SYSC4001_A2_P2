////////////////////////////////////////////////////////////////// Code for #5 ////////////////////////////////////////////////////////
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <csignal>

//note: the sembuf struct is defined in <sys/sem.h> and looks like this:
//already in header file
// struct sembuf {
//     unsigned short sem_num;  // Which semaphore in the set
//     short sem_op;            // Operation (-1 = wait, +1 = signal, etc.)
//     short sem_flg;           // Flags (0, IPC_NOWAIT, SEM_UNDO)
// };

//What is the difference between semctl and semop? 
//semop takes in a struct &sb which contains the operation to be performed on the semaphor, it also
//takes the semid and the number of operations to be performed (1 in this case). If you make sb.sem_op = -1.
//it will do the wait() operation." On the other hand, semctl is used for intializaiion, it allows you to directly
//set the value of the semaphor using the SETVAL command. 
//General rule:
//--> Use semctl for initialization and control operations (like setting values, removing semaphores).
//--> Use semop for performing wait and signal operations during process synchronization. and you have to use
//the sembuf struct with it (which is already included in sys/sem.h).

// Union required for semctl initialization
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// Wait (P) operation
void sem_wait_custom(int semid) {
    struct sembuf sb = {0, -1, 0}; // short hand initalization
    semop(semid, &sb, 1); //calls the wait or signal operation on the semaphor depending on how the sb is initialized
}

// Signal (V) operation
void sem_signal_custom(int semid) {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}

int main(void) {
    // Shared key for semaphore and shared memory
    key_t sem_key = 65;
    key_t shm_key = 1234;

    // Create semaphore
    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid < 0) {
        std::cerr << "Failed to create semaphore\n";
        exit(1);
    }

    // Initialize semaphore to 1 (unlocked)
    union semun sem_union;
    sem_union.val = 1;
    semctl(semid, 0, SETVAL, sem_union);

    // Shared memory
    int shmid = shmget(shm_key, 2 * sizeof(int), 0666 | IPC_CREAT);
    if (shmid < 0) {
        std::cerr << "Failed to create shared memory\n";
        exit(1);
    }

    int* shared_mem = (int*)shmat(shmid, nullptr, 0);
    shared_mem[0] = 3; // multiple
    shared_mem[1] = 0; // counter

    // Fork child 1
    pid_t pid1 = fork();
    if (pid1 < 0) {
        std::cerr << "Fork failed\n";
        exit(1);
    } else if (pid1 == 0) {
        int cycle_counter1 = 0;
        std::cout << "Child 1 started\n";
        while (shared_mem[1] < 500) {
            sem_wait_custom(semid);
            shared_mem[1]++;
            if (shared_mem[1] % shared_mem[0] == 0)
                std::cout << "Cycle " << cycle_counter1++ << " | Child1 Counter: " 
                          << shared_mem[1] << " (multiple of " << shared_mem[0] << ")\n";
            else
                std::cout << "Cycle " << cycle_counter1++ << " | Child1 Counter: " 
                          << shared_mem[1] << "\n";
            sem_signal_custom(semid);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        exit(0);
    }

    // Fork child 2 (process2)
    pid_t pid2 = fork();
    if (pid2 < 0) {
        std::cerr << "Fork failed\n";
        exit(1);
    } else if (pid2 == 0) {
        execl("./process2", "process2", nullptr);
        exit(1); // only reached if execl fails
    }

    // Wait for process2 to finish
    int status;
    waitpid(pid2, &status, 0);

    // Cleanup
    kill(pid1, SIGTERM);
    semctl(semid, 0, IPC_RMID, sem_union);
    shmdt(shared_mem);
    shmctl(shmid, IPC_RMID, nullptr);

    return 0;
}
