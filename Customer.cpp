#include "Customer.h"
#include <sstream>
#include <vector>

int Customer::nextId = 1;

Customer::Customer() : id(0), name(""), email(""), phone("") {}

Customer::Customer(const std::string& name, const std::string& email, const std::string& phone)
    : id(nextId++), name(name), email(email), phone(phone) {}

Customer::Customer(int id, const std::string& name, const std::string& email, const std::string& phone)
    : id(id), name(name), email(email), phone(phone) {
    if (id >= nextId) nextId = id + 1;
}

void Customer::display() const {
    std::cout << "Customer ID: " << id << "\n"
              << "Name: " << name << "\n"
              << "Email: " << email << "\n"
              << "Phone: " << phone << "\n";
}

std::string Customer::serialize() const {
    std::stringstream ss;
    ss << id << "|" << name << "|" << email << "|" << phone;
    return ss.str();
}

Customer Customer::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() >= 4) {
        return Customer(std::stoi(tokens[0]), tokens[1], tokens[2], tokens[3]);
    }
    return Customer();
}