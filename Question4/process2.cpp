#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <chrono>
#include <thread>

int main() {
    key_t shm_key = 1234;

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
        shared_mem[1]--;
        if (shared_mem[1] % shared_mem[0] == 0)
            std::cout << "Cycle " << cycle_counter++ << " | Child2 Counter: "
                      << shared_mem[1] << " (multiple of " << shared_mem[0] << ")\n";
        else
            std::cout << "Cycle " << cycle_counter++ << " | Child2 Counter: "
                      << shared_mem[1] << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    shmdt(shared_mem);
    return 0;
}
