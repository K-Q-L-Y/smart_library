#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>
#include <iostream>
#include <list>

// Helper functions
static std::string formatCellP(std::string text, size_t width) {
    if (text.length() > width) return text.substr(0, width - 3) + "...";
    return text + std::string(width - text.length(), ' ');
}

static std::string formatDateP(long long t) {
    time_t timer = (time_t)t;
    struct tm* timeInfo = localtime(&timer);
    if (!timeInfo) return "Unknown";
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%b %d %Y", timeInfo);
    return std::string(buffer);
}

// Base Class
class Person {
protected:
    std::string id;
    std::string name;
    std::string email;

public:
    Person(std::string id, std::string name, std::string email);
    virtual ~Person() {}

    std::string getId() const;
    std::string getName() const;
	std::string getEmail() const;
    
    virtual std::string getRole() const = 0; 
    virtual std::string toFileString() const = 0;
};

// Derived Class: Librarian
class Librarian : public Person {
public:
    Librarian(std::string id, std::string name, std::string email);
    std::string getRole() const override;
    std::string toFileString() const override;
};

// Derived Class: Member
class Member : public Person {
private:
    std::list<std::string> borrowingHistory; 

public:
    Member(std::string id, std::string name, std::string email);
    std::string getRole() const override;
    
	void addToHistory(std::string bookTitle, std::string action);
    void displayHistory() const;
    std::string toFileString() const override;
    
    void loadHistory(const std::string& historyStr);
};

// Derived Class: Guest
class Guest : public Person {
public:
    Guest();
    std::string getRole() const override;
    std::string toFileString() const override;
};

#endif