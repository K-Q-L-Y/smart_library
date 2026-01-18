#include "LibrarySystem.hpp"
#include <iostream>

int main() {
    LibrarySystem app;
    
    while (app.run());

    std::cout << "Program Terminated. Goodbye!\n";
    return 0;
}