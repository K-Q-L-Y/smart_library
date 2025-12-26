#include "LibrarySystem.hpp"
#include <iostream>
#include <limits>

int main() {
    LibrarySystem app;
    
    // Loop controls
    int c;
    
    while(true) {
        app.run();
        
    //     while(true) {
    //         std::cout << "\nPerform another action? (1: Yes, 0: Exit System): ";
    //         std::cin >> c;
            
    //         if (std::cin.fail()) {
    //             std::cin.clear();
    //             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //             std::cout << "Invalid input. Please enter 1 or 0.\n";
    //         } else {
    //             if(c == 0 || c == 1) {
    //                 break;
    //             } else {
    //                 std::cout << "Invalid option. Please enter 1 or 0.\n";
    //             }
    //         }
    //     }

    //     if (c == 0) {
    //         std::cout << "Exiting system. Goodbye!\n";
    //         break;
    //     }
    }

    return 0;
}