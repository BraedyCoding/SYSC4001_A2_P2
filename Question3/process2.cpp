#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>

int main(void) {
    std::cout << "Process 2 (PID " << getpid() << ") started.\n";

    int counter1 = 0;
    int cycle_counter = 0;

    while (true) {
        if (counter1 % 3 == 0)
            std::cout << "Cycle number " << cycle_counter++
                      << " | Process 2 Counter: " << counter1
                      << " - Multiple of 3!\n";
        else
            std::cout << "Cycle number " << cycle_counter++
                      << " | Process 2 Counter: " << counter1 << std::endl;

        counter1--;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (counter1 < -500) {
            std::cout << "\nProcess 2 reached -500. Exiting...\n";
            exit(0);
        }
    }
}
