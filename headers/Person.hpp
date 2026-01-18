#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>
#include <iostream>
#include <list>

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
    
    void addToHistory(std::string bookTitle);
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