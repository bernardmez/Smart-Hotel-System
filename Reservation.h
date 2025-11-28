#ifndef RESERVATION_H
#define RESERVATION_H

#include <ctime>
#include <string>
#include <iostream>

enum class ReservationStatus {
    PENDING,
    CONFIRMED,
    CHECKED_IN,
    CHECKED_OUT,
    CANCELLED
};

class Reservation {
private:
    int id;
    int customerId;
    time_t checkInTime;
    time_t checkOutTime;
    int assignedRoomNumber;
    ReservationStatus status;
    double totalCost;
    static int nextId;

public:
    Reservation();
    Reservation(int customerId, time_t checkIn, time_t checkOut);
    Reservation(int id, int customerId, time_t checkIn, time_t checkOut,
                int roomNumber, ReservationStatus status, double cost);

    int getId() const { return id; }
    int getCustomerId() const { return customerId; }
    time_t getCheckInTime() const { return checkInTime; }
    time_t getCheckOutTime() const { return checkOutTime; }
    int getAssignedRoomNumber() const { return assignedRoomNumber; }
    ReservationStatus getStatus() const { return status; }
    double getTotalCost() const { return totalCost; }

    void setAssignedRoomNumber(int roomNumber) { assignedRoomNumber = roomNumber; }
    void setStatus(ReservationStatus s) { status = s; }
    void setTotalCost(double cost) { totalCost = cost; }
    static void setNextId(int id) { nextId = id; }

    bool overlaps(const Reservation& other) const;
    int getDuration() const;

    std::string getStatusString() const;
    void display() const;

    std::string serialize() const;
    static Reservation deserialize(const std::string& data);
};

#endif
