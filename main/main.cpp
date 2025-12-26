#include "LibrarySystem.hpp"
#include <iostream>

int main() {
    LibrarySystem app;
    
    // The run() method returns true to continue (logout), 
    // and false to exit the program.
    while (app.run());

    std::cout << "Program Terminated. Goodbye!\n";
    return 0;
}