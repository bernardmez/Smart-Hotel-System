#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
#include <iostream>

enum class RoomType {
    SINGLE,
    DOUBLE,
    SUITE,
    DELUXE
};

class Room {
private:
    int roomNumber;
    RoomType type;
    double pricePerNight;
    std::vector<int> reservationIds; // Track which reservations are assigned to this room
    
public:
    Room();
    Room(int number, RoomType type, double price);
    
    // Getters
    int getRoomNumber() const { return roomNumber; }
    RoomType getType() const { return type; }
    double getPricePerNight() const { return pricePerNight; }
    const std::vector<int>& getReservationIds() const { return reservationIds; }
    
    // Setters
    void setPricePerNight(double price) { pricePerNight = price; }
    
    // Reservation management
    void addReservation(int reservationId);
    void removeReservation(int reservationId);
    
    // Display
    void display() const;
    std::string getTypeString() const;
    
    // File I/O
    std::string serialize() const;
    static Room deserialize(const std::string& data);
};

#endif