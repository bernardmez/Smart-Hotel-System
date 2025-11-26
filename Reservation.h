#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>
#include <ctime>
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
    
    // Getters
    int getId() const { return id; }
    int getCustomerId() const { return customerId; }
    time_t getCheckInTime() const { return checkInTime; }
    time_t getCheckOutTime() const { return checkOutTime; }
    int getAssignedRoomNumber() const { return assignedRoomNumber; }
    ReservationStatus getStatus() const { return status; }
    double getTotalCost() const { return totalCost; }
    
    // Setters
    void setAssignedRoomNumber(int room) { assignedRoomNumber = room; }
    void setStatus(ReservationStatus s) { status = s; }
    void setTotalCost(double cost) { totalCost = cost; }
    void setCheckInTime(time_t t) { checkInTime = t; }
    void setCheckOutTime(time_t t) { checkOutTime = t; }
    
    // Utility
    bool overlaps(const Reservation& other) const;
    int getDuration() const; // in days
    std::string getStatusString() const;
    
    // Display
    void display() const;
    
    // File I/O
    std::string serialize() const;
    static Reservation deserialize(const std::string& data);
    static void setNextId(int id) { nextId = id; }
};

#endif