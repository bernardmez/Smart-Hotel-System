#include "Scheduler.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

Scheduler::Scheduler(std::vector<Room>& roomsRef,
                     std::vector<Reservation>& reservationsRef)
    : rooms(roomsRef), reservations(reservationsRef) {}

bool Scheduler::canAssignToRoom(const Reservation& res, const Room& room) const {
    // Check if this reservation overlaps with any reservation already in this room
    for (int resId : room.getReservationIds()) {
        for (const auto& existingRes : reservations) {
            if (existingRes.getId() == resId &&
                existingRes.getStatus() != ReservationStatus::CANCELLED &&
                existingRes.getStatus() != ReservationStatus::CHECKED_OUT) {
                if (res.overlaps(existingRes)) {
                    return false;
                }
            }
        }
    }
    return true;
}

// Helper to assign a reservation to a concrete room
static void assignToRoom(Reservation& res, Room& room) {
    room.addReservation(res.getId());
    res.setAssignedRoomNumber(room.getRoomNumber());
    res.setStatus(ReservationStatus::CONFIRMED);
    double cost = res.getDuration() * room.getPricePerNight();
    res.setTotalCost(cost);
}

void Scheduler::scheduleReservations() {
    // Left Edge algorithm over ALL rooms (no type preference)
    std::vector<Reservation*> active;
    active.reserve(reservations.size());

    for (auto& r : reservations) {
        if (r.getStatus() == ReservationStatus::CANCELLED ||
            r.getStatus() == ReservationStatus::CHECKED_OUT) {
            continue;
        }
        active.push_back(&r);
    }

    std::sort(active.begin(), active.end(),
              [](const Reservation* a, const Reservation* b) {
                  return a->getCheckInTime() < b->getCheckInTime();
              });

    for (auto* resPtr : active) {
        Reservation& res = *resPtr;
        bool assigned = false;

        for (auto& room : rooms) {
            if (canAssignToRoom(res, room)) {
                assignToRoom(res, room);
                assigned = true;
                break;
            }
        }

        if (!assigned) {
            res.setAssignedRoomNumber(-1);
            res.setStatus(ReservationStatus::PENDING);
            res.setTotalCost(0.0);
        }
    }
}

bool Scheduler::scheduleReservation(int reservationId) {
    // Old behaviour: no type filter
    Reservation* res = nullptr;
    for (auto& r : reservations) {
        if (r.getId() == reservationId) {
            res = &r;
            break;
        }
    }
    if (!res) return false;

    for (auto& room : rooms) {
        if (canAssignToRoom(*res, room)) {
            assignToRoom(*res, room);
            return true;
        }
    }

    res->setAssignedRoomNumber(-1);
    res->setStatus(ReservationStatus::PENDING);
    res->setTotalCost(0.0);
    return false;
}

bool Scheduler::scheduleReservation(int reservationId, RoomType preferredType) {
    // NEW behaviour: only consider rooms of the chosen type
    Reservation* res = nullptr;
    for (auto& r : reservations) {
        if (r.getId() == reservationId) {
            res = &r;
            break;
        }
    }
    if (!res) return false;

    // Left-Edge style: first suitable room of that type
    for (auto& room : rooms) {
        if (room.getType() != preferredType) continue;
        if (canAssignToRoom(*res, room)) {
            assignToRoom(*res, room);
            return true;
        }
    }

    // No room of that type available in that time range
    res->setAssignedRoomNumber(-1);
    res->setStatus(ReservationStatus::PENDING);
    res->setTotalCost(0.0);
    return false;
}

void Scheduler::removeReservation(int reservationId) {
    // Find reservation
    Reservation* res = nullptr;
    for (auto& r : reservations) {
        if (r.getId() == reservationId) {
            res = &r;
            break;
        }
    }
    if (!res) return;

    // Remove from its room (if any)
    if (res->getAssignedRoomNumber() != -1) {
        for (auto& room : rooms) {
            if (room.getRoomNumber() == res->getAssignedRoomNumber()) {
                room.removeReservation(reservationId);
                break;
            }
        }
    }

    res->setAssignedRoomNumber(-1);
    res->setStatus(ReservationStatus::CANCELLED);
    res->setTotalCost(0.0);
}

void Scheduler::displaySchedule() const {
    std::cout << "\n====== COMPLETE ROOM SCHEDULE ======\n";
    for (const auto& room : rooms) {
        std::cout << "Room " << room.getRoomNumber()
                  << " (" << room.getTypeString() << ", $"
                  << room.getPricePerNight() << "/night)\n";

        if (room.getReservationIds().empty()) {
            std::cout << "  [No reservations]\n\n";
            continue;
        }

        for (int resId : room.getReservationIds()) {
            for (const auto& r : reservations) {
                if (r.getId() == resId) {
                    std::cout << "  - Reservation #" << r.getId()
                              << " [Customer " << r.getCustomerId() << "] "
                              << r.getStatusString() << "\n";
                    break;
                }
            }
        }
        std::cout << "\n";
    }
}

void Scheduler::displayRoomSchedule(int roomNumber) const {
    std::cout << "\n====== SCHEDULE FOR ROOM " << roomNumber << " ======\n";

    const Room* roomPtr = nullptr;
    for (const auto& room : rooms) {
        if (room.getRoomNumber() == roomNumber) {
            roomPtr = &room;
            break;
        }
    }

    if (!roomPtr) {
        std::cout << "Room not found.\n";
        return;
    }

    const Room& room = *roomPtr;

    if (room.getReservationIds().empty()) {
        std::cout << "No reservations for this room.\n";
        return;
    }

    for (int resId : room.getReservationIds()) {
        for (const auto& r : reservations) {
            if (r.getId() == resId) {
                r.display();
                std::cout << "\n";
                break;
            }
        }
    }
}

int Scheduler::getTotalRoomsUsed() const {
    int count = 0;
    for (const auto& room : rooms) {
        bool used = false;
        for (int resId : room.getReservationIds()) {
            for (const auto& r : reservations) {
                if (r.getId() == resId &&
                    r.getStatus() != ReservationStatus::CANCELLED &&
                    r.getStatus() != ReservationStatus::CHECKED_OUT) {
                    used = true;
                    break;
                }
            }
            if (used) break;
        }
        if (used) ++count;
    }
    return count;
}

double Scheduler::getOccupancyRate() const {
    if (rooms.empty()) return 0.0;
    return (static_cast<double>(getTotalRoomsUsed()) / rooms.size()) * 100.0;
}

std::map<int, std::vector<int>> Scheduler::getRoomAssignments() const {
    std::map<int, std::vector<int>> assignments;
    for (const auto& room : rooms) {
        assignments[room.getRoomNumber()] = room.getReservationIds();
    }
    return assignments;
}
