#ifndef LIBRARYSYSTEM_HPP
#define LIBRARYSYSTEM_HPP

#include "Book.hpp"
#include "Person.hpp"
#include <vector>

class LibrarySystem {
private:
    std::vector<Book> books;
    std::vector<Person*> users; // Polymorphic container 
    const std::string bookFile = "books.txt";
    const std::string userFile = "users.txt";

    // Helpers
    Person* findUser(std::string id);
    Book* findBook(std::string id);
    void calculateFine(time_t dueDate);

public:
    LibrarySystem();
    ~LibrarySystem(); // Destructor to clean up pointers

    void loadData(); // 
    void saveData(); // 

    // Menu Operations [cite: 23]
    void run();
    void librarianMenu(Librarian* lib);
    void memberMenu(Member* mem);
    void guestMenu();

    // Core Features
    void addBook();
    void viewAllBooks();
    void removeBook();
    void registerMember();
    void removeMember();
    void borrowBook(Member* mem);
    void returnBook(Member* mem);
    void searchBooks();
    void displayBorrowedBooks();
};

#endif