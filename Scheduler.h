#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Reservation.h"
#include "Room.h"
#include <vector>
#include <map>

class Scheduler {
private:
    std::vector<Room>& rooms;
    std::vector<Reservation>& reservations;
    
    // Helper function to check if a reservation can fit in a room
    bool canAssignToRoom(const Reservation& res, const Room& room) const;
    
public:
    Scheduler(std::vector<Room>& rooms, std::vector<Reservation>& reservations);
    
    // Left Edge Algorithm implementation
    void scheduleReservations();
    
    // Schedule a single new reservation
    bool scheduleReservation(int reservationId);
    
    // Remove a reservation from schedule
    void removeReservation(int reservationId);
    
    // Display schedule
    void displaySchedule() const;
    void displayRoomSchedule(int roomNumber) const;
    
    // Get statistics
    int getTotalRoomsUsed() const;
    double getOccupancyRate() const;
    std::map<int, std::vector<int>> getRoomAssignments() const;
};

#endif