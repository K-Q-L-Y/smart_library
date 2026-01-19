#include "LibrarySystem.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits> 
#include <cctype> 
#include <iomanip>
#include <ctime>

/* HELPERS */
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

std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), tolower);
    return lowerStr;
}

/* FORMATTING */
// Truncates text with "..." if too long, or adds spaces if too short
std::string formatCell(std::string text, size_t width) {
    if (text.length() > width) {
        return text.substr(0, width - 3) + "...";
    }
    return text + std::string(width - text.length(), ' ');
}

// Format time_t to string "Jan 1 2025"
std::string formatDate(time_t t) {
    struct tm* timeInfo = localtime(&t);
    if (!timeInfo) return "Unknown"; // Safety check
    
    char buffer[20];
    // %b = Abbreviated Month, %d = Day, %Y = Year
    strftime(buffer, sizeof(buffer), "%b %d %Y", timeInfo); 
    return std::string(buffer);
}

void printTitle() {
    std::cout << "=======================================\n";
    std::cout << "   SMART LIBRARY MANAGEMENT SYSTEM     \n";
    std::cout << "=======================================\n";
}

// Header for listing books
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

// Print books
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

/* Constructor and Destructor */

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

/* File Persistence */
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

/* Additional helpers */
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

/* Menus */
bool LibrarySystem::run() {
    printTitle();
	std::cout << "Login as: \n";
    std::cout << "1. Librarian\n";
    std::cout << "2. Member\n";
    std::cout << "3. Guest (Seach only)\n";
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
        std::cout << "Invalid choice.\n\n";
        return true; 
    }

	std::cout << "Logging in as: " << ((choice == 1) ? "Librarian\n" : "Member\n");
    std::string id;
    std::cout << "Enter User ID: ";
    std::cin >> id;

    Person* user = findUser(id);
    if (!user) {
        std::cout << "[Error] Invalid ID. (Hint: Try 'admin' if first run)\n\n";
        return true; 
    }

    if (choice == 1 && dynamic_cast<Librarian*>(user)) {
        librarianMenu(dynamic_cast<Librarian*>(user));
    } else if (choice == 2 && dynamic_cast<Member*>(user)) {
        memberMenu(dynamic_cast<Member*>(user));
    } else {
        std::cout << "[Error] Access Denied or Wrong Role.\n\n";
    }

    return true; 
}

void LibrarySystem::librarianMenu(Librarian* lib) {
	std::system("clear");
	printTitle();
    int choice;
    do {
        std::cout << "--- Librarian Menu (" << lib->getName() << ") ---\n";
        std::cout << "1. Add Book\t\t4. Add User\t\t0. Logout\n";
		std::cout << "2. Remove Book\t\t5. Remove User\n";
		std::cout << "3. Display all books\t6. Display all users\nChoice: ";
        
        choice = getValidInt();

        switch (choice) {
            case 1: addBook(); break;
            case 2: removeBook(); break;
            case 3: displayAllBooks(); break;
            case 4: registerUser(); break; 
            case 5: removeUser(); break;
			case 6: displayAllUsers(); break;
            case 0: std::cout << "Logging out...\n\n"; break;
            default: std::cout << "Invalid option.\n\n";
        }
    } while (choice != 0);
}

void LibrarySystem::memberMenu(Member* mem) {
	system("clear");
	printTitle();
    int choice;
    do {
        std::cout << "--- Member Menu (" << mem->getName() << ") ---\n";
        std::cout << "1. Search Books\n2. Borrow Book\n3. Return Book\n4. View Borrowed Books\n5. View History\n0. Logout\nChoice: ";
        
        choice = getValidInt();

        switch (choice) {
            case 1: searchBooks(); break;
            case 2: borrowBook(mem); break;
            case 3: returnBook(mem); break;
			case 4: displayBorrowedBooks(mem); break;
            case 5: mem->displayHistory(); break;
            case 0: std::cout << "Logging out...\n\n"; break;
            default: std::cout << "Invalid option.\n\n";
        }
    } while (choice != 0);
}

void LibrarySystem::guestMenu() {
	std::system("clear");
	printTitle();
    std::cout << "--- Guest Menu ---\n";
	std::string input;
    
	while(true) {
		searchBooks();
		std::cout << "Search for another book? (y/n): ";
		std::cin >> input;
		if (input != "y" && input != "Y")
			break;
	}

    std::cout << "\nGuest access finished. Please register to borrow books.\n\n";
}

/* Core Functionalities */
void LibrarySystem::addBook() {
	std::system("clear");
	printTitle();
    std::string id, title, author, genre;
    
	int tmp_id = 1;
	for (auto it = books.begin(); it != books.end(); ++it) {
		if (stoi(it->getId()) != tmp_id)
			break;
		tmp_id++;
	}
    id = std::to_string(tmp_id);
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

void LibrarySystem::removeBook() {
	std::system("clear");
	printTitle();
	displayAllBooks();

    std::string id;
    std::cout << "Enter Book ID to remove: "; 
    std::cin >> id; 
    
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

void LibrarySystem::displayAllBooks() {
	std::system("clear");
	printTitle();
	std::cout << "--- Registered Books ---\n";
    if(books.empty()) {
        std::cout << "No books in library.\n";
        return;
    }

	std::cout << std::string(115, '-') << "\n";
	std::cout << formatCell("ID", 8) << " | "
	<< formatCell("Title", 30) << " | "
	<< formatCell("Author", 20) << " | "
	<< formatCell("Genre", 15) << " | "
	<< formatCell("Due Date", 15) << " | "
	<< "Borrower ID\n";
	std::cout << std::string(115, '-') << "\n";

	for (const auto& b : books) {
			std::string dateStr = formatDate(b.getDueDate());

			std::cout << formatCell(b.getId(), 8) << " | "
						<< formatCell(b.getTitle(), 30) << " | "
						<< formatCell(b.getAuthor(), 20) << " | "
						<< formatCell(b.getGenre(), 15) << " | "
						<< formatCell(dateStr, 15) << " | "
						<< (b.getBorrowedById().empty() ? "N/A" : b.getBorrowedById()) << "\n";
		}
	std::cout << std::string(115, '-') << "\n";
}

void LibrarySystem::registerUser() {
	std::system("clear");
	printTitle();
    std::cout << "Register New Account:\n1. Member\n2. Librarian\nChoice: ";
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

void LibrarySystem::removeUser() {
	std::system("clear");
	printTitle();

	displayAllUsers();
    std::string id;
    std::cout << "Enter User ID to remove: "; std::cin >> id;
    
    if (id == "admin") {
        std::cout << "[Error] Cannot remove the default admin account.\n";
        return;
    }

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

void LibrarySystem::displayAllUsers() {
	std::system("clear");
	printTitle();
    std::cout << "--- Registered Users ---\n";
    std::cout << std::string(90, '-') << "\n";
    std::cout << formatCell("ID", 10) << " | "
              << formatCell("Name", 30) << " | "
              << formatCell("Email", 30) << " | "
              << "Role\n";
    std::cout << std::string(90, '-') << "\n";

    for (const auto& user : users) {
        std::cout << formatCell(user->getId(), 10) << " | "
                  << formatCell(user->getName(), 30) << " | "
                  << formatCell(user->getEmail(), 30) << " | "
                  << user->getRole() << "\n";
    }
    std::cout << std::string(90, '-') << "\n";
}

bool LibrarySystem::searchBooks() {
	std::system("clear");
	printTitle();
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
	std::system("clear");
	printTitle();
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
    else {
        book->borrowBook(mem->getId(), 7); 
        mem->addToHistory(book->getTitle(), "Borrowed");
        std::cout << "Book borrowed successfully.\n";
    }
}

void LibrarySystem::returnBook(Member* mem) {
	std::system("clear");
	printTitle();
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

	displayBorrowedBooks(mem);

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

	// Give book to reserved person
	if (book->hasReservations()) {
		std::string nextUser = book->processNextReservation();
		Member *tmp = dynamic_cast<Member*>(findUser(nextUser));
		book->borrowBook(tmp->getId(), 7); 
        mem->addToHistory(book->getTitle(), "Borrowed");
	}
	else
	    book->returnBook();

    mem->addToHistory(book->getTitle(), "Returned");
    std::cout << "Book returned successfully.\n";
}

void LibrarySystem::displayBorrowedBooks(Member *mem) {
	std::system("clear");
	printTitle();
	std::vector<Book*> myBooks;
	for (auto& b : books) {
		if (b.getBorrowedById() == mem->getId()) {
			myBooks.push_back(&b);
		}
	}

	if (myBooks.empty()) {
		std::cout << "You currently have no borrowed books.\n\n";
		return;
	}

	std::cout << "Your Borrowed Books:\n";
	printHeader();
	for (Book* b : myBooks) {
		printBookRow(*b);
	}
	std::cout << std::string(110, '-') << "\n\n";
}