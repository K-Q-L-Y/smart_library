#include "LibrarySystem.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits> 
#include <cctype> 
#include <iomanip> // Required for formatting
#include <ctime>   // Required for strftime and localtime

// --- Helper for Robust Input ---
int getValidInt() {
    int choice;
    while (true) {
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cout << "Invalid input. Please enter a number: ";
        } else {
            return choice;
        }
    }
}

// --- Helper for Case-Insensitive Search ---
std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), tolower);
    return lowerStr;
}

// --- FORMATTING HELPERS ---

// Truncates text with "..." if too long, or adds spaces if too short
std::string formatCell(std::string text, size_t width) {
    if (text.length() > width) {
        return text.substr(0, width - 3) + "...";
    }
    return text + std::string(width - text.length(), ' ');
}

// Helper to format time_t to "Dec 27 2025"
std::string formatDate(time_t t) {
    struct tm* timeInfo = localtime(&t);
    if (!timeInfo) return "Unknown"; // Safety check
    
    char buffer[20];
    // %b = Abbreviated Month, %d = Day, %Y = Year
    strftime(buffer, sizeof(buffer), "%b %d %Y", timeInfo); 
    return std::string(buffer);
}

void printHeader() {
    std::cout << std::string(110, '-') << "\n";
    std::cout << formatCell("ID", 8) << " | "
              << formatCell("Title", 30) << " | "
              << formatCell("Author", 20) << " | "
              << formatCell("Genre", 15) << " | "
              << formatCell("Status", 10) << " | "
              << "Due Date\n";
    std::cout << std::string(110, '-') << "\n";
}

void printBookRow(const Book& b) {
    std::string status = b.getIsBorrowed() ? "Borrowed" : "Available";
    std::string dueDateStr = "-";

    if (b.getIsBorrowed()) {
        dueDateStr = formatDate(b.getDueDate());
    }

    std::cout << formatCell(b.getId(), 8) << " | "
              << formatCell(b.getTitle(), 30) << " | "
              << formatCell(b.getAuthor(), 20) << " | "
              << formatCell(b.getGenre(), 15) << " | "
              << formatCell(status, 10) << " | "
              << dueDateStr << "\n";
}

// ---------------------------

LibrarySystem::LibrarySystem() {
    loadData();
}

LibrarySystem::~LibrarySystem() {
    saveData();
    for (auto user : users) {
        delete user;
    }
    users.clear();
}

// --- File Persistence ---
void LibrarySystem::saveData() {
    std::ofstream bOut(bookFile);
    for (const auto& book : books) {
        bOut << book.toFileString() << "\n";
    }
    bOut.close();

    std::ofstream uOut(userFile);
    for (const auto& user : users) {
        if (user->getRole() != "Guest") {
            uOut << user->toFileString() << "\n";
        }
    }
    uOut.close();
}

void LibrarySystem::loadData() {
    std::ifstream bIn(bookFile);
    std::string line;
    while (std::getline(bIn, line)) {
        if(line.empty()) continue;
        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> seglist;
        while (std::getline(ss, segment, '|')) {
            seglist.push_back(segment);
        }
        if (seglist.size() >= 7) {
            Book b(seglist[0], seglist[1], seglist[2], seglist[3], static_cast<time_t>(std::stoll(seglist[5])));
            std::cout << "bookid: " << seglist[0] << " load duedate: " << static_cast<time_t>(std::stoll(seglist[5])) << std::endl;
			bool borrowed = (seglist[4] == "1");
            std::string borrower = seglist[6];
            
            if (borrowed) b.borrowBook(borrower, 0, static_cast<time_t>(std::stoll(seglist[5]))); 

            if (seglist.size() > 7) {
                b.loadReservationsFromString(seglist[7]);
            }
            books.push_back(b);
        }
    }
    bIn.close();

    std::ifstream uIn(userFile);
    while (std::getline(uIn, line)) {
        if(line.empty()) continue;
        std::stringstream ss(line);
        std::string type, id, name, email, history;
        std::getline(ss, type, '|');
        std::getline(ss, id, '|');
        std::getline(ss, name, '|');
        std::getline(ss, email, '|');

        if (type == "Librarian") {
            users.push_back(new Librarian(id, name, email));
        } else if (type == "Member") {
            Member* m = new Member(id, name, email);
            if (std::getline(ss, history, '|')) {
                m->loadHistory(history);
            }
            users.push_back(m);
        }
    }
    uIn.close();

    if (users.empty()) {
        std::cout << "[System] No users found. Creating Default Admin account.\n";
        std::cout << "[System] ID: admin | Name: Admin\n"; 
        users.push_back(new Librarian("admin", "Admin", "admin@library.com"));
    }
}

// --- Helpers ---
Person* LibrarySystem::findUser(std::string id) {
    for (auto user : users) {
        if (user->getId() == id) return user;
    }
    return nullptr;
}

Book* LibrarySystem::findBook(std::string id) {
    for (auto& book : books) {
        if (book.getId() == id) return &book;
    }
    return nullptr;
}

void LibrarySystem::calculateFine(time_t dueDate) {
    time_t now = time(0);
    double seconds = difftime(now, dueDate);
    if (seconds > 0) {
        int daysOverdue = (int)(seconds / (60 * 60 * 24));
        std::cout << "[!] Book is overdue by " << daysOverdue << " days.\n";
        std::cout << "Fine: $" << daysOverdue * 0.50 << "\n"; 
    } else {
        std::cout << "Returned on time. No fine.\n";
    }
}

// --- Menus ---
bool LibrarySystem::run() {
    std::cout << "\n=======================================\n";
    std::cout << "   SMART LIBRARY MANAGEMENT SYSTEM     \n";
    std::cout << "=======================================\n";
    std::cout << "1. Login as Librarian\n";
    std::cout << "2. Login as Member\n";
    std::cout << "3. Guest Access\n";
    std::cout << "0. Exit Program\n";
    std::cout << "---------------------------------------\n";
    std::cout << "Choice: ";
    
    int choice = getValidInt();

    if (choice == 0) return false; 

    if (choice == 3) {
        guestMenu();
        return true; 
    }

    if (choice != 1 && choice != 2) {
        std::cout << "Invalid choice.\n";
        return true; 
    }

    std::string id;
    std::cout << "Enter User ID: ";
    std::cin >> id;

    Person* user = findUser(id);
    if (!user) {
        std::cout << "[Error] Invalid ID. (Hint: Try 'admin' if first run)\n";
        return true; 
    }

    if (choice == 1 && dynamic_cast<Librarian*>(user)) {
        librarianMenu(dynamic_cast<Librarian*>(user));
    } else if (choice == 2 && dynamic_cast<Member*>(user)) {
        memberMenu(dynamic_cast<Member*>(user));
    } else {
        std::cout << "[Error] Access Denied or Wrong Role.\n";
    }

    return true; 
}

void LibrarySystem::librarianMenu(Librarian* lib) {
    int choice;
    do {
        std::cout << "\n--- Librarian Menu (" << lib->getName() << ") ---\n";
        std::cout << "1. Add Book\n2. Remove Book\n3. Register New User\n4. Remove User\n";
        std::cout << "5. View All Books\n6. Display Borrowed Books\n0. Logout\nChoice: ";
        
        choice = getValidInt();

        switch (choice) {
            case 1: addBook(); break;
            case 2: removeBook(); break;
            case 3: registerMember(); break; 
            case 4: removeMember(); break;
            case 5: viewAllBooks(); break;
            case 6: displayBorrowedBooks(); break;
            case 0: std::cout << "Logging out...\n"; break;
            default: std::cout << "Invalid option.\n";
        }
    } while (choice != 0);
}

void LibrarySystem::memberMenu(Member* mem) {
    int choice;
    do {
        std::cout << "\n--- Member Menu (" << mem->getName() << ") ---\n";
        std::cout << "1. Search Books\n2. Borrow Book\n3. Return Book\n4. View History\n0. Logout\nChoice: ";
        
        choice = getValidInt();

        switch (choice) {
            case 1: searchBooks(); break;
            case 2: borrowBook(mem); break;
            case 3: returnBook(mem); break;
            case 4: mem->displayHistory(); break;
            case 0: std::cout << "Logging out...\n"; break;
            default: std::cout << "Invalid option.\n";
        }
    } while (choice != 0);
}

void LibrarySystem::guestMenu() {
    std::cout << "\n--- Guest Menu ---\n";
    searchBooks(); 
    std::cout << "\nGuest access finished. Please register to borrow books.\n";
}

// --- Core Functionalities ---

void LibrarySystem::addBook() {
    std::string id, title, author, genre;
    
    std::cout << "Enter Book ID: "; 
    std::cin >> id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter Title: "; 
    std::getline(std::cin, title);
    std::cout << "Enter Author: "; 
    std::getline(std::cin, author);
    std::cout << "Enter Genre: "; 
    std::getline(std::cin, genre);

    books.emplace_back(id, title, author, genre, time(0));
    std::cout << "Book added successfully.\n";
}

void LibrarySystem::viewAllBooks() {
    if(books.empty()) {
        std::cout << "No books in library.\n";
        return;
    }
    // Using new table format
    printHeader();
    for (const auto& b : books) {
        printBookRow(b);
    }
    std::cout << std::string(110, '-') << "\n";
}

void LibrarySystem::removeBook() {
    std::string id;
    std::cout << "Enter Book ID to remove: "; 
    std::cin >> id; 
    
    // CHANGED: List-specific erasure
    bool found = false;
    for (auto it = books.begin(); it != books.end(); ++it) {
        if (it->getId() == id) {
            books.erase(it);
            found = true;
            std::cout << "Book removed.\n";
            break; 
        }
    }
    
    if (!found) std::cout << "Book not found.\n";
}

void LibrarySystem::registerMember() {
    std::cout << "Register New Account:\n1. Library Member\n2. Librarian (Staff)\nChoice: ";
    int type = getValidInt();
    
    if (type != 1 && type != 2) {
        std::cout << "Invalid account type selected.\n";
        return;
    }

    std::string id, name, email;
    std::cout << "Enter New ID: "; std::cin >> id;
    
    if (findUser(id) != nullptr) {
        std::cout << "[Error] User ID already exists!\n";
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "Enter Name: "; std::getline(std::cin, name);
    std::cout << "Enter Email: "; std::cin >> email;
    
    if (type == 1) {
        users.push_back(new Member(id, name, email));
        std::cout << "Member registered successfully.\n";
    } else {
        users.push_back(new Librarian(id, name, email));
        std::cout << "Librarian registered successfully.\n";
    }
}

void LibrarySystem::removeMember() {
    std::string id;
    std::cout << "Enter User ID to remove: "; std::cin >> id;
    
    if (id == "admin") {
        std::cout << "[Error] Cannot remove the default admin account.\n";
        return;
    }

    // CHANGED: List-specific erasure
    bool found = false;
    for (auto it = users.begin(); it != users.end(); ++it) {
        if ((*it)->getId() == id) {
            delete *it; // Free memory
            users.erase(it); // Remove node
            found = true;
            std::cout << "User removed.\n";
            break;
        }
    }

    if (!found) std::cout << "User not found.\n";
}

bool LibrarySystem::searchBooks() {
    std::string query;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "Search (Title/Author/Genre): "; 
    std::getline(std::cin, query);

    std::string queryLower = toLower(query);

    bool found = false;
    bool headerPrinted = false;

    for (const auto& b : books) {
        if (toLower(b.getId()).find(queryLower) != std::string::npos ||
			toLower(b.getTitle()).find(queryLower) != std::string::npos ||
            toLower(b.getAuthor()).find(queryLower) != std::string::npos ||
            toLower(b.getGenre()).find(queryLower) != std::string::npos) {
            
            if (!headerPrinted) {
				std::cout << "Search Results:\n";
				printHeader();
				headerPrinted = true;
            }
            printBookRow(b);
            found = true;
        }
    }
    if(headerPrinted) std::cout << std::string(110, '-') << "\n";
    if (!found) std::cout << "No matching books found.\n";
	return found;
}

void LibrarySystem::borrowBook(Member* mem) {
    std::cout << "\n--- Find a Book to Borrow ---\n";
    
    if (!searchBooks()) return;

    std::string bookId;
    std::cout << "\n--- Enter Book ID ---\n";
    std::cout << "Enter Book ID to borrow (or enter '0' to cancel): "; 
    std::cin >> bookId;

    if (bookId == "0") {
        std::cout << "Borrowing cancelled.\n";
        return;
    }

    Book* book = findBook(bookId);
    if (!book) {
        std::cout << "[Error] Book does not exist.\n";
        return;
    }

    if (book->getIsBorrowed()) {
        std::cout << "Book is currently unavailable.\n";
        char ch;
        std::cout << "Do you want to reserve it? (y/n): ";
        std::cin >> ch;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (ch == 'y' || ch == 'Y') {
            book->addReservation(mem->getId()); // todo
            std::cout << "You have been added to the reservation queue.\n";
        }
    }
	// else { // todo: can remove this, add to returnbook so that when return can process
    //     if (book->hasReservations()) {
    //         std::string nextUser = book->processNextReservation();
    //         if (nextUser != mem->getId()) {
    //             std::cout << "This book is reserved for user: " << nextUser << ". You cannot borrow it yet.\n";
    //             return; 
    //         }
    //     }
    else {
        book->borrowBook(mem->getId(), 7); 
        mem->addToHistory(book->getTitle() + " (Borrowed)"); 
        std::cout << "Book borrowed successfully.\n";
    }
}

void LibrarySystem::returnBook(Member* mem) {
    std::cout << "\n--- Return a Book ---\n";

    // Filter books borrowed by this specific member
    std::vector<Book*> myBooks;
    for (auto& b : books) {
        if (b.getBorrowedById() == mem->getId()) {
            myBooks.push_back(&b);
        }
    }

    if (myBooks.empty()) {
        std::cout << "You currently have no borrowed books to return.\n";
        return;
    }

    // Display them nicely
    std::cout << "Your Borrowed Books:\n";
    printHeader();
    for (Book* b : myBooks) {
        printBookRow(*b);
    }
    std::cout << std::string(110, '-') << "\n";

    // Prompt
    std::string bookId;
    std::cout << "Enter Book ID to return (or enter '0' to cancel): "; 
    std::cin >> bookId;

    if (bookId == "0") {
        std::cout << "Return cancelled.\n";
        return;
    }

    Book* book = findBook(bookId);
    if (!book) {
        std::cout << "[Error] Invalid Book ID.\n"; 
        return;
    }

    // Double check ownership (security)
    if (book->getBorrowedById() != mem->getId()) {
        std::cout << "[Error] You did not borrow this book (ID: " << bookId << ").\n"; 
        return;
    }

    calculateFine(book->getDueDate());

	// settle reservations
	if (book->hasReservations()) {
		std::string nextUser = book->processNextReservation();
		Member *tmp = dynamic_cast<Member*>(findUser(nextUser));
		book->borrowBook(tmp->getId(), 7); 
        tmp->addToHistory(book->getTitle() + " (Borrowed)"); 
	}
	else
	    book->returnBook();
    mem->addToHistory(book->getTitle() + " (Returned)");
    std::cout << "Book returned successfully.\n";
}

void LibrarySystem::displayBorrowedBooks() {
    std::cout << "--- Currently Borrowed Books ---\n";
    
    std::cout << std::string(115, '-') << "\n";
    std::cout << formatCell("ID", 8) << " | "
              << formatCell("Title", 30) << " | "
              << formatCell("Author", 20) << " | "
              << formatCell("Genre", 15) << " | "
              << formatCell("Due Date", 15) << " | "
              << "Borrower ID\n";
    std::cout << std::string(115, '-') << "\n";

    bool any = false;
    for (const auto& b : books) {
        if (b.getIsBorrowed()) {
            std::string dateStr = formatDate(b.getDueDate());

            std::cout << formatCell(b.getId(), 8) << " | "
                      << formatCell(b.getTitle(), 30) << " | "
                      << formatCell(b.getAuthor(), 20) << " | "
                      << formatCell(b.getGenre(), 15) << " | "
                      << formatCell(dateStr, 15) << " | "
                      << b.getBorrowedById() << "\n";
            any = true;
        }
    }
    std::cout << std::string(115, '-') << "\n";
    if(!any) std::cout << "No books are currently borrowed.\n";
}