#include "Book.hpp"
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

Book::Book(std::string id, std::string title, std::string author, std::string genre, time_t dueDate)
    : id(id), title(title), author(author), genre(genre), isBorrowed(false), dueDate(dueDate), borrowedByMemberId("") {}

std::string Book::getId() const { return id; }
std::string Book::getTitle() const { return title; }
std::string Book::getAuthor() const { return author; }
std::string Book::getGenre() const { return genre; }
bool Book::getIsBorrowed() const { return isBorrowed; }
time_t Book::getDueDate() const { return dueDate; }
std::string Book::getBorrowedById() const { return borrowedByMemberId; }

#include <iostream>
void Book::borrowBook(std::string memberId, int daysToBorrow, time_t due) {
    isBorrowed = true;
    borrowedByMemberId = memberId;
    // Set due date to current time + days
	if (due)
		dueDate = due;
	else
	    dueDate = time(0) + (daysToBorrow * 24 * 60 * 60);
	std::cout << "dueDate: " << dueDate << std::endl;
}

void Book::returnBook() {
    isBorrowed = false;
    borrowedByMemberId = "";
    dueDate = 0;
}

void Book::addReservation(std::string memberId) {
    reservationQueue.push(memberId);
}

std::string Book::processNextReservation() {
    if (reservationQueue.empty()) return "";
    std::string nextMember = reservationQueue.front();
    reservationQueue.pop();
    return nextMember;
}

bool Book::hasReservations() const {
    return !reservationQueue.empty();
}

std::string Book::toString() const {
    std::stringstream ss;
    ss << "ID: " << id << " | Title: " << title << " | Author: " << author 
       << " | Status: " << (isBorrowed ? "Borrowed" : "Available");
    
    if (isBorrowed) {
        char* dt = ctime(&dueDate); 
        std::string dateStr = dt ? dt : "Unknown";
        
		// remove newline
        if (!dateStr.empty() && dateStr.back() == '\n')
            dateStr.pop_back();
        
        ss << " | Due: " << dateStr;
    }
    return ss.str();
}

std::string Book::toFileString() const {
    std::stringstream ss;
    ss << id << "|" << title << "|" << author << "|" << genre << "|" 
       << isBorrowed << "|" << dueDate << "|" << borrowedByMemberId << "|";
    
    // Save queue data
    std::queue<std::string> tempQ = reservationQueue;
    while(!tempQ.empty()) {
        ss << tempQ.front();
        tempQ.pop();
        if(!tempQ.empty()) ss << ",";
    }
    return ss.str();
}

void Book::loadReservationsFromString(const std::string& data) {
    std::stringstream ss(data);
    std::string memberId;
    while(std::getline(ss, memberId, ',')) {
        if(!memberId.empty()) reservationQueue.push(memberId);
    }
}