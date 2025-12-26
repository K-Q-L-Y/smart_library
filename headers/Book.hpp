#ifndef BOOK_HPP
#define BOOK_HPP

#include <string>
#include <queue>
#include <ctime>

class Book {
private:
    std::string id;
    std::string title;
    std::string author;
    std::string genre;
    bool isBorrowed;
    time_t dueDate;
    std::string borrowedByMemberId;
    std::queue<std::string> reservationQueue; // 

public:
    Book(std::string id, std::string title, std::string author, std::string genre);
    
    // Getters
    std::string getId() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    std::string getGenre() const;
    bool getIsBorrowed() const;
    time_t getDueDate() const;
    std::string getBorrowedById() const;

    // Setters and Operations
    void borrowBook(std::string memberId, int daysToBorrow);
    void returnBook();
    void addReservation(std::string memberId);
    std::string processNextReservation(); // Returns member ID of next in line
    bool hasReservations() const;
    
    // Formatting helpers
    std::string toString() const; // For display
    std::string toFileString() const; // For text file storage
    void loadReservationsFromString(const std::string& data); // Helper to load queue
};

#endif