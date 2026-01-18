#include "Person.hpp"
#include <sstream>

// --- Person ---
Person::Person(std::string id, std::string name, std::string email) 
    : id(id), name(name), email(email) {}

std::string Person::getId() const { return id; }
std::string Person::getName() const { return name; }
std::string Person::getEmail() const { return email; }

// --- Librarian ---
Librarian::Librarian(std::string id, std::string name, std::string email) 
    : Person(id, name, email) {}

std::string Librarian::getRole() const { return "Librarian"; }

std::string Librarian::toFileString() const {
    return "Librarian|" + id + "|" + name + "|" + email;
}

// --- Member ---
Member::Member(std::string id, std::string name, std::string email) 
    : Person(id, name, email) {}

std::string Member::getRole() const { return "Member"; }

void Member::addToHistory(std::string bookTitle) {
    borrowingHistory.push_back(bookTitle); // Add to linked list
}

void Member::displayHistory() const {
    std::cout << "Borrowing History for " << name << ":\n";
    if (borrowingHistory.empty()) {
        std::cout << " - No history available.\n";
        return;
    }
    for (const auto& entry : borrowingHistory) {
        std::cout << " - " << entry << "\n";
    }
}

std::string Member::toFileString() const {
    std::stringstream ss;
    ss << "Member|" << id << "|" << name << "|" << email << "|";
    // Serialize linked list
    for (auto it = borrowingHistory.begin(); it != borrowingHistory.end(); ++it) {
        ss << *it;
        if (std::next(it) != borrowingHistory.end()) ss << ",";
    }
    return ss.str();
}

void Member::loadHistory(const std::string& historyStr) {
    std::stringstream ss(historyStr);
    std::string item;
    while(std::getline(ss, item, ',')) {
        if(!item.empty()) borrowingHistory.push_back(item);
    }
}

// --- Guest ---
Guest::Guest() : Person("GUEST", "Guest User", "N/A") {}

std::string Guest::getRole() const { return "Guest"; }
std::string Guest::toFileString() const { return ""; } // Guests are not saved