#include "Room.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace {
    bool extractJsonNumber(const std::string& json, const std::string& key, long long& out) {
        std::string pattern = "\"" + key + "\"";
        size_t pos = json.find(pattern);
        if (pos == std::string::npos) return false;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return false;
        ++pos;
        while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) ++pos;
        size_t end = pos;
        while (end < json.size() &&
               (std::isdigit(static_cast<unsigned char>(json[end])) ||
                json[end] == '-' || json[end] == '+' ||
                json[end] == '.' || json[end] == 'e' || json[end] == 'E')) {
            ++end;
        }
        if (end == pos) return false;
        try {
            out = std::stoll(json.substr(pos, end - pos));
        } catch (...) {
            return false;
        }
        return true;
    }
}

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
    std::ostringstream ss;
    ss << "{";
    ss << "\"roomNumber\":" << roomNumber << ",";
    ss << "\"type\":" << static_cast<int>(type) << ",";
    ss << "\"pricePerNight\":" << pricePerNight << ",";
    ss << "\"reservationIds\":[";
    for (size_t i = 0; i < reservationIds.size(); ++i) {
        if (i > 0) ss << ",";
        ss << reservationIds[i];
    }
    ss << "]";
    ss << "}";
    return ss.str();
}

Room Room::deserialize(const std::string& data) {
    Room room;
    long long roomNum = 0;
    long long typeVal = 0;
    double priceVal = 0.0;

    if (extractJsonNumber(data, "roomNumber", roomNum)) {
        room.roomNumber = static_cast<int>(roomNum);
    }
    if (extractJsonNumber(data, "type", typeVal)) {
        room.type = static_cast<RoomType>(static_cast<int>(typeVal));
    }

    {
        std::string key = "\"pricePerNight\"";
        size_t pos = data.find(key);
        if (pos != std::string::npos) {
            pos = data.find(':', pos);
            if (pos != std::string::npos) {
                ++pos;
                while (pos < data.size() && std::isspace(static_cast<unsigned char>(data[pos]))) ++pos;
                size_t end = pos;
                while (end < data.size() &&
                       (std::isdigit(static_cast<unsigned char>(data[end])) ||
                        data[end] == '-' || data[end] == '+' ||
                        data[end] == '.' || data[end] == 'e' || data[end] == 'E')) {
                    ++end;
                }
                try {
                    priceVal = std::stod(data.substr(pos, end - pos));
                } catch (...) {
                    priceVal = 0.0;
                }
            }
        }
    }
    room.pricePerNight = priceVal;

    std::string key = "\"reservationIds\"";
    size_t pos = data.find(key);
    if (pos != std::string::npos) {
        pos = data.find('[', pos);
        if (pos != std::string::npos) {
            ++pos;
            size_t end = data.find(']', pos);
            if (end != std::string::npos && end > pos) {
                std::string list = data.substr(pos, end - pos);
                std::stringstream ss(list);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    try {
                        int val = std::stoi(item);
                        room.reservationIds.push_back(val);
                    } catch (...) {
                    }
                }
            }
        }
    }

    return room;
}
