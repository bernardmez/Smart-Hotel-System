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
    std::vector<int> reservationIds;

public:
    Room();
    Room(int number, RoomType type, double price);

    int getRoomNumber() const { return roomNumber; }
    RoomType getType() const { return type; }
    double getPricePerNight() const { return pricePerNight; }
    const std::vector<int>& getReservationIds() const { return reservationIds; }

    void addReservation(int reservationId);
    void removeReservation(int reservationId);

    void display() const;
    std::string getTypeString() const;

    std::string serialize() const;
    static Room deserialize(const std::string& data);
};

#endif
