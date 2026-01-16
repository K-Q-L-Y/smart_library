#ifndef LIBRARYSYSTEM_HPP
#define LIBRARYSYSTEM_HPP

#include "Book.hpp"
#include "Person.hpp"
#include <list>
#include <vector>

class LibrarySystem {
private:
    std::list<Book> books;
    std::list<Person*> users; // Polymorphic container 
    const std::string bookFile = "data/books.txt";
    const std::string userFile = "data/users.txt";

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
    bool run();
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
    bool searchBooks();
    void displayBorrowedBooks(Member *mem = 0);
};

#endif