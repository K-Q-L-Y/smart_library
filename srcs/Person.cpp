#include "Person.hpp"
#include <sstream>
#include <vector>

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

void Member::addToHistory(std::string bookTitle, std::string action) {
    std::stringstream ss;
    ss << time(0) << "|" << action << "|" << bookTitle;
    borrowingHistory.push_back(ss.str());
}

void Member::displayHistory() const {
	std::system("clear");
    std::cout << "=======================================\n";
    std::cout << "   SMART LIBRARY MANAGEMENT SYSTEM     \n";
    std::cout << "=======================================\n";

    std::cout << "History for " << name << ":\n";
    if (borrowingHistory.empty()) {
        std::cout << " - No history available.\n\n";
        return;
    }

    std::cout << std::string(60, '-') << "\n";
    std::cout << formatCellP("Date", 15) << " | "
              << formatCellP("Action", 10) << " | "
              << "Book Title\n";
    std::cout << std::string(60, '-') << "\n";

    for (const auto& entry : borrowingHistory) {
        std::stringstream ss(entry);
        std::string segment;
        std::vector<std::string> parts;
        
        // Parse "Timestamp|Action|Title"
        while(std::getline(ss, segment, '|')) {
            parts.push_back(segment);
        }

        if (parts.size() >= 3) {
            std::string dateStr = formatDateP(std::stoll(parts[0]));
            std::cout << formatCellP(dateStr, 15) << " | "
                      << formatCellP(parts[1], 10) << " | "
                      << parts[2] << "\n";
        } else {
            // Fallback for old data format
            std::cout << entry << "\n"; 
        }
    }
    std::cout << std::string(60, '-') << "\n\n";
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