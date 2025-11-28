#ifndef HOTEL_SYSTEM_H
#define HOTEL_SYSTEM_H

#include "Customer.h"
#include "Reservation.h"
#include "Room.h"
#include "Scheduler.h"
#include <vector>
#include <string>
#include <memory>
#include <ctime>


class HotelSystem {
private:
    std::vector<Customer> customers;
    std::vector<Reservation> reservations;
    std::vector<Room> rooms;
    std::unique_ptr<Scheduler> scheduler;
    
    std::string customersFile = "customers.json";
    std::string reservationsFile = "reservations.json";
    std::string roomsFile = "rooms.json";
    
    std::string adminUsername = "admin";
    std::string adminPassword = "admin123";
    bool isAdminLoggedIn = false;
    
    void loadData();
    void saveData();
    void loadCustomers();
    void loadReservations();
    void loadRooms();
    void saveCustomers();
    void saveReservations();
    void saveRooms();
    
    Customer* findCustomer(int id);
    Reservation* findReservation(int id);
    Room* findRoom(int roomNumber);
    time_t createDateTime(int year, int month, int day, int hour = 14, int minute = 0);
    void displayMenu();
    void displayAdminMenu();

public:
    HotelSystem();
    ~HotelSystem();
    
    void run();
    void initialize();
    
    void addCustomer();
    void viewCustomers();
    void modifyCustomer();
    void deleteCustomer();
    
    void createReservation();
    void viewReservations();
    void cancelReservation();
    void checkInReservation();
    void checkOutReservation();
    
    void viewRooms();
    void viewRoomSchedule();
    
    void generateInvoice(int reservationId);
    
    bool adminLogin();
    void adminLogout();
    void viewOccupancyReport();
    void createBackup();
    void restoreBackup();
    
    void rescheduleAll();
};

#endif
