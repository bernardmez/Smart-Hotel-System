#include "Room.h"
#include <sstream>
#include <algorithm>

Room::Room() : roomNumber(0), type(RoomType::SINGLE), pricePerNight(0.0) {}

Room::Room(int number, RoomType type, double price)
    : roomNumber(number), type(type), pricePerNight(price) {}

void Room::addReservation(int reservationId) {
    reservationIds.push_back(reservationId);
}

void Room::removeReservation(int reservationId) {
    auto it = std::find(reservationIds.begin(), reservationIds.end(), reservationId);
    if (it != reservationIds.end()) {
        reservationIds.erase(it);
    }
}

std::string Room::getTypeString() const {
    switch (type) {
        case RoomType::SINGLE: return "Single";
        case RoomType::DOUBLE: return "Double";
        case RoomType::SUITE: return "Suite";
        case RoomType::DELUXE: return "Deluxe";
        default: return "Unknown";
    }
}

void Room::display() const {
    std::cout << "Room Number: " << roomNumber << "\n"
              << "Type: " << getTypeString() << "\n"
              << "Price per Night: $" << pricePerNight << "\n"
              << "Active Reservations: " << reservationIds.size() << "\n";
}

std::string Room::serialize() const {
    std::stringstream ss;
    ss << roomNumber << "|" << static_cast<int>(type) << "|" << pricePerNight << "|";
    
    for (size_t i = 0; i < reservationIds.size(); ++i) {
        ss << reservationIds[i];
        if (i < reservationIds.size() - 1) ss << ",";
    }
    
    return ss.str();
}

Room Room::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }
    
    Room room;
    if (tokens.size() >= 3) {
        room.roomNumber = std::stoi(tokens[0]);
        room.type = static_cast<RoomType>(std::stoi(tokens[1]));
        room.pricePerNight = std::stod(tokens[2]);
        
        if (tokens.size() > 3 && !tokens[3].empty()) {
            std::stringstream resStream(tokens[3]);
            std::string resId;
            while (std::getline(resStream, resId, ',')) {
                room.reservationIds.push_back(std::stoi(resId));
            }
        }
    }
    
    return room;
}