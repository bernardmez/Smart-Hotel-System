#include "Scheduler.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

Scheduler::Scheduler(std::vector<Room>& rooms, std::vector<Reservation>& reservations)
    : rooms(rooms), reservations(reservations) {}

bool Scheduler::canAssignToRoom(const Reservation& res, const Room& room) const {
    // Check if this reservation overlaps with any reservation already in this room
    for (int resId : room.getReservationIds()) {
        for (const auto& existingRes : reservations) {
            if (existingRes.getId() == resId && 
                existingRes.getStatus() != ReservationStatus::CANCELLED) {
                if (res.overlaps(existingRes)) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Scheduler::scheduleReservations() {
    // Left Edge Algorithm implementation
    
    // Step 1: Sort reservations by check-in time
    std::vector<Reservation*> sortedReservations;
    for (auto& res : reservations) {
        if (res.getStatus() != ReservationStatus::CANCELLED) {
            sortedReservations.push_back(&res);
        }
    }
    
    std::sort(sortedReservations.begin(), sortedReservations.end(),
              [](const Reservation* a, const Reservation* b) {
                  return a->getCheckInTime() < b->getCheckInTime();
              });
    
    // Step 2: For each reservation, try to place it in the first available room
    for (auto* res : sortedReservations) {
        bool assigned = false;
        
        // Try to assign to existing rooms
        for (auto& room : rooms) {
            if (canAssignToRoom(*res, room)) {
                res->setAssignedRoomNumber(room.getRoomNumber());
                room.addReservation(res->getId());
                
                // Calculate cost
                double cost = res->getDuration() * room.getPricePerNight();
                res->setTotalCost(cost);
                
                if (res->getStatus() == ReservationStatus::PENDING) {
                    res->setStatus(ReservationStatus::CONFIRMED);
                }
                
                assigned = true;
                break;
            }
        }
        
        if (!assigned) {
            std::cout << "Warning: Cannot assign reservation " << res->getId() 
                      << " - no available rooms!\n";
        }
    }
}

bool Scheduler::scheduleReservation(int reservationId) {
    Reservation* targetRes = nullptr;
    
    for (auto& res : reservations) {
        if (res.getId() == reservationId) {
            targetRes = &res;
            break;
        }
    }
    
    if (!targetRes || targetRes->getStatus() == ReservationStatus::CANCELLED) {
        return false;
    }
    
    // Try to assign to existing rooms
    for (auto& room : rooms) {
        if (canAssignToRoom(*targetRes, room)) {
            targetRes->setAssignedRoomNumber(room.getRoomNumber());
            room.addReservation(targetRes->getId());
            
            double cost = targetRes->getDuration() * room.getPricePerNight();
            targetRes->setTotalCost(cost);
            
            if (targetRes->getStatus() == ReservationStatus::PENDING) {
                targetRes->setStatus(ReservationStatus::CONFIRMED);
            }
            
            return true;
        }
    }
    
    return false;
}

void Scheduler::removeReservation(int reservationId) {
    for (auto& room : rooms) {
        room.removeReservation(reservationId);
    }
    
    for (auto& res : reservations) {
        if (res.getId() == reservationId) {
            res.setStatus(ReservationStatus::CANCELLED);
            res.setAssignedRoomNumber(-1);
            break;
        }
    }
}

void Scheduler::displaySchedule() const {
    std::cout << "\n========== HOTEL SCHEDULE ==========\n";
    std::cout << "Total Rooms Used: " << getTotalRoomsUsed() << " / " << rooms.size() << "\n";
    std::cout << "Occupancy Rate: " << std::fixed << std::setprecision(1) 
              << getOccupancyRate() << "%\n\n";
    
    for (const auto& room : rooms) {
        if (!room.getReservationIds().empty()) {
            std::cout << "Room " << room.getRoomNumber() 
                      << " (" << room.getTypeString() << ") - "
                      << room.getReservationIds().size() << " reservation(s)\n";
            
            for (int resId : room.getReservationIds()) {
                for (const auto& res : reservations) {
                    if (res.getId() == resId) {
                        time_t checkIn = res.getCheckInTime();   // <-- FIX 3: Local variable
                        time_t checkOut = res.getCheckOutTime(); // <-- FIX 3: Local variable
                        
                        std::cout << "  " << std::setw(5) << res.getId()
                                << std::put_time(std::localtime(&checkIn), "%Y-%m-%d") // <-- FIX 3: Address of local variable
                                << " to " 
                                << std::put_time(std::localtime(&checkOut), "%Y-%m-%d") // <-- FIX 3: Address of local variable
                                << " [" << res.getStatusString() << "]\n";
                    }
                }
            }
            std::cout << "\n";
        }
    }
}

void Scheduler::displayRoomSchedule(int roomNumber) const {
    for (const auto& room : rooms) {
        if (room.getRoomNumber() == roomNumber) {
            std::cout << "\n========== Room " << roomNumber << " Schedule ==========\n";
            room.display();
            
            std::cout << "\nReservations:\n";
            for (int resId : room.getReservationIds()) {
                for (const auto& res : reservations) {
                    if (res.getId() == resId) {
                        std::cout << "---\n";
                        res.display();
                    }
                }
            }
            return;
        }
    }
    std::cout << "Room not found.\n";
}

int Scheduler::getTotalRoomsUsed() const {
    int count = 0;
    for (const auto& room : rooms) {
        if (!room.getReservationIds().empty()) {
            count++;
        }
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