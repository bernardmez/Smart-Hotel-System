#include "Reservation.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>

int Reservation::nextId = 1;

Reservation::Reservation() 
    : id(0), customerId(0), checkInTime(0), checkOutTime(0), 
      assignedRoomNumber(-1), status(ReservationStatus::PENDING), totalCost(0.0) {}

Reservation::Reservation(int customerId, time_t checkIn, time_t checkOut)
    : id(nextId++), customerId(customerId), checkInTime(checkIn), 
      checkOutTime(checkOut), assignedRoomNumber(-1), 
      status(ReservationStatus::PENDING), totalCost(0.0) {}

Reservation::Reservation(int id, int customerId, time_t checkIn, time_t checkOut,
                         int roomNumber, ReservationStatus status, double cost)
    : id(id), customerId(customerId), checkInTime(checkIn), 
      checkOutTime(checkOut), assignedRoomNumber(roomNumber), 
      status(status), totalCost(cost) {
    if (id >= nextId) nextId = id + 1;
}

bool Reservation::overlaps(const Reservation& other) const {
    return !(checkOutTime <= other.checkInTime || checkInTime >= other.checkOutTime);
}

int Reservation::getDuration() const {
    double diffHours = std::difftime(checkOutTime, checkInTime) / 3600.0;
    return static_cast<int>(std::ceil(diffHours / 24.0));
}

std::string Reservation::getStatusString() const {
    switch (status) {
        case ReservationStatus::PENDING: return "Pending";
        case ReservationStatus::CONFIRMED: return "Confirmed";
        case ReservationStatus::CHECKED_IN: return "Checked In";
        case ReservationStatus::CHECKED_OUT: return "Checked Out";
        case ReservationStatus::CANCELLED: return "Cancelled";
        default: return "Unknown";
    }
}

void Reservation::display() const {
    std::cout << "Reservation ID: " << id << "\n"
              << "Customer ID: " << customerId << "\n"
              << "Check-in: " << std::put_time(std::localtime(&checkInTime), "%Y-%m-%d %H:%M") << "\n"
              << "Check-out: " << std::put_time(std::localtime(&checkOutTime), "%Y-%m-%d %H:%M") << "\n"
              << "Duration: " << getDuration() << " days\n"
              << "Assigned Room: " << (assignedRoomNumber > 0 ? std::to_string(assignedRoomNumber) : "Not Assigned") << "\n"
              << "Status: " << getStatusString() << "\n"
              << "Total Cost: $" << totalCost << "\n";
}

std::string Reservation::serialize() const {
    std::stringstream ss;
    ss << id << "|" << customerId << "|" << checkInTime << "|" << checkOutTime << "|"
       << assignedRoomNumber << "|" << static_cast<int>(status) << "|" << totalCost;
    return ss.str();
}

Reservation Reservation::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() >= 7) {
        return Reservation(
            std::stoi(tokens[0]),
            std::stoi(tokens[1]),
            std::stoll(tokens[2]),
            std::stoll(tokens[3]),
            std::stoi(tokens[4]),
            static_cast<ReservationStatus>(std::stoi(tokens[5])),
            std::stod(tokens[6])
        );
    }
    return Reservation();
}