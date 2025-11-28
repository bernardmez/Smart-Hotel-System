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

    bool canAssignToRoom(const Reservation& res, const Room& room) const;

public:
    Scheduler(std::vector<Room>& rooms, std::vector<Reservation>& reservations);

    void scheduleReservations();

    bool scheduleReservation(int reservationId);

    bool scheduleReservation(int reservationId, RoomType preferredType);

    void removeReservation(int reservationId);

    void displaySchedule() const;
    void displayRoomSchedule(int roomNumber) const;

    int getTotalRoomsUsed() const;
    double getOccupancyRate() const;
    std::map<int, std::vector<int>> getRoomAssignments() const;
};

#endif
