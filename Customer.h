#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <iostream>

class Customer {
private:
    int id;
    std::string name;
    std::string email;
    std::string phone;
    static int nextId;

public:
    Customer();
    Customer(const std::string& name, const std::string& email, const std::string& phone);
    Customer(int id, const std::string& name, const std::string& email, const std::string& phone);
    
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getPhone() const { return phone; }
    
    void setName(const std::string& n) { name = n; }
    void setEmail(const std::string& e) { email = e; }
    void setPhone(const std::string& p) { phone = p; }
    
    void display() const;
    
    std::string serialize() const;
    static Customer deserialize(const std::string& data);
    static void setNextId(int id) { nextId = id; }
};

#endif
