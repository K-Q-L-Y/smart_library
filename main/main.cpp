#include "LibrarySystem.hpp"
#include <iostream>

int main() {
	std::system("clear");
    LibrarySystem app;
    
    while (app.run());

    std::cout << "Program Terminated. Goodbye!\n";
    return 0;
}