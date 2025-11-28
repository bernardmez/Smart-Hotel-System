#include "Reservation.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cctype>

int Reservation::nextId = 1;

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

    bool extractJsonDouble(const std::string& json, const std::string& key, double& out) {
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
            out = std::stod(json.substr(pos, end - pos));
        } catch (...) {
            return false;
        }
        return true;
    }
}

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
    std::tm in_tm  = *std::localtime(&checkInTime);
    std::tm out_tm = *std::localtime(&checkOutTime);

    in_tm.tm_hour = in_tm.tm_min = in_tm.tm_sec = 0;
    out_tm.tm_hour = out_tm.tm_min = out_tm.tm_sec = 0;

    time_t in_midnight  = std::mktime(&in_tm);
    time_t out_midnight = std::mktime(&out_tm);

    if (out_midnight <= in_midnight) {
        return 0;  // defensive: invalid or zero-length stay
    }

    double diffDays = std::difftime(out_midnight, in_midnight) / 86400.0;
    return static_cast<int>(diffDays);  // exact whole number of nights
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
    time_t in = checkInTime;
    time_t out = checkOutTime;
    std::cout << "Reservation ID: " << id << "\n"
              << "Customer ID: " << customerId << "\n"
              << "Check-in: " << std::put_time(std::localtime(&in), "%Y-%m-%d %H:%M") << "\n"
              << "Check-out: " << std::put_time(std::localtime(&out), "%Y-%m-%d %H:%M") << "\n"
              << "Duration: " << getDuration() << " days\n"
              << "Assigned Room: " << (assignedRoomNumber > 0 ? std::to_string(assignedRoomNumber) : "Not Assigned") << "\n"
              << "Status: " << getStatusString() << "\n"
              << "Total Cost: $" << totalCost << "\n";
}

std::string Reservation::serialize() const {
    std::ostringstream ss;
    ss << "{";
    ss << "\"id\":" << id << ",";
    ss << "\"customerId\":" << customerId << ",";
    ss << "\"checkInTime\":" << static_cast<long long>(checkInTime) << ",";
    ss << "\"checkOutTime\":" << static_cast<long long>(checkOutTime) << ",";
    ss << "\"assignedRoomNumber\":" << assignedRoomNumber << ",";
    ss << "\"status\":" << static_cast<int>(status) << ",";
    ss << "\"totalCost\":" << totalCost;
    ss << "}";
    return ss.str();
}

Reservation Reservation::deserialize(const std::string& data) {
    long long idVal = 0;
    long long customerIdVal = 0;
    long long checkInVal = 0;
    long long checkOutVal = 0;
    long long roomVal = -1;
    long long statusVal = 0;
    double costVal = 0.0;

    if (!extractJsonNumber(data, "id", idVal)) {
        return Reservation();
    }
    extractJsonNumber(data, "customerId", customerIdVal);
    extractJsonNumber(data, "checkInTime", checkInVal);
    extractJsonNumber(data, "checkOutTime", checkOutVal);
    extractJsonNumber(data, "assignedRoomNumber", roomVal);
    extractJsonNumber(data, "status", statusVal);
    extractJsonDouble(data, "totalCost", costVal);

    return Reservation(
        static_cast<int>(idVal),
        static_cast<int>(customerIdVal),
        static_cast<time_t>(checkInVal),
        static_cast<time_t>(checkOutVal),
        static_cast<int>(roomVal),
        static_cast<ReservationStatus>(static_cast<int>(statusVal)),
        costVal
    );
}
