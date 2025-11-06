//////CODE FROM PART 2 and 3 ///////////////////////////

#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>   // <-- needed for waitpid
#include <cstdlib>      // for exit
#include <chrono>
#include <thread>       // for std::this_thread::sleep_for


int main(void){


    std::cout << "Child process is running" << std::endl;
    // Child process 2
    int counter1 = 0;
    int cycle_counter = 0;

     while (true) {

        if(counter1 % 3 == 0) {
            std::cout<<"Cycle Number "<<cycle_counter++<<" Process 2 Counter: "<<counter1--<<" - Multiple of 3!"<<std::endl;
        }
        else{   
             std::cout << "Cycle number " << cycle_counter++ << " Process 2 Counter: " << counter1--<< std::endl;
        }
        
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); // slow down output

        
    }
}
