#include "HotelSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <limits>
#include <ctime>
#include <cstring>
#include <algorithm>

HotelSystem::HotelSystem() {
    initialize();
}

HotelSystem::~HotelSystem() {
    saveData();
}
static bool isDigitsOnly(const std::string& s) {
    if (s.empty()) return false;
    for (unsigned char ch : s) {
        if (!std::isdigit(ch)) return false;
    }
    return true;
}

static bool isValidName(const std::string& name) {
    if (name.empty()) return false;

    bool hasAlpha = false;
    for (unsigned char ch : name) {
        if (std::isalpha(ch)) {
            hasAlpha = true;
        }
    }
    // Must have at least one letter and not be all digits
    if (!hasAlpha) return false;
    if (isDigitsOnly(name)) return false;

    return true;
}

static bool isValidEmail(const std::string& email) {
    if (email.empty()) return false;
    if (email.find(' ') != std::string::npos) return false;

    auto atPos = email.find('@');
    if (atPos == std::string::npos || atPos == 0 || atPos == email.size() - 1)
        return false;

    auto dotPos = email.find('.', atPos + 1);
    if (dotPos == std::string::npos || dotPos == atPos + 1 || dotPos == email.size() - 1)
        return false;

    return true;
}

static bool isValidPhone(const std::string& phone) {
    if (phone.empty()) return false;

    std::size_t i = 0;
    if (phone[0] == '+') {
        if (phone.size() == 1) return false;
        i = 1;
    }

    int digits = 0;
    for (; i < phone.size(); ++i) {
        unsigned char ch = phone[i];
        if (!std::isdigit(ch)) return false;
        ++digits;
    }

    // basic reasonable range
    return digits >= 7 && digits <= 15;
}

void HotelSystem::initialize() {
    if (rooms.empty()) {
        rooms.push_back(Room(101, RoomType::SINGLE, 100.0));
        rooms.push_back(Room(102, RoomType::SINGLE, 100.0));
        rooms.push_back(Room(201, RoomType::DOUBLE, 150.0));
        rooms.push_back(Room(202, RoomType::DOUBLE, 150.0));
        rooms.push_back(Room(301, RoomType::SUITE, 250.0));
        rooms.push_back(Room(302, RoomType::DELUXE, 350.0));
    }
    
    loadData();
    scheduler = std::make_unique<Scheduler>(rooms, reservations);
}

void HotelSystem::loadData() {
    loadCustomers();
    loadReservations();
    loadRooms();
}

void HotelSystem::saveData() {
    saveCustomers();
    saveReservations();
    saveRooms();
}

void HotelSystem::loadCustomers() {
    std::ifstream file(customersFile);
    if (!file.is_open()) return;
    
    customers.clear();
    
    std::string line;
    int maxId = 0;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            Customer c = Customer::deserialize(line);
            if (c.getId() > maxId) maxId = c.getId();
            customers.push_back(c);
        }
    }
    Customer::setNextId(maxId + 1);
    file.close();
}

void HotelSystem::loadReservations() {
    std::ifstream file(reservationsFile);
    if (!file.is_open()) return;
    
    reservations.clear();
    
    std::string line;
    int maxId = 0;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            Reservation r = Reservation::deserialize(line);
            if (r.getId() > maxId) maxId = r.getId();
            reservations.push_back(r);
        }
    }
    Reservation::setNextId(maxId + 1);
    file.close();
}

void HotelSystem::loadRooms() {
    std::ifstream file(roomsFile);
    if (!file.is_open()) {
        saveRooms();
        return;
    }
    
    rooms.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            rooms.push_back(Room::deserialize(line));
        }
    }
    file.close();
}

void HotelSystem::saveCustomers() {
    std::ofstream file(customersFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << customersFile << " for writing.\n";
        return;
    }
    for (const auto& c : customers) {
        file << c.serialize() << "\n";
    }
    file.close();
}

void HotelSystem::saveReservations() {
    std::ofstream file(reservationsFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << reservationsFile << " for writing.\n";
        return;
    }
    for (const auto& r : reservations) {
        file << r.serialize() << "\n";
    }
    file.close();
}

void HotelSystem::saveRooms() {
    std::ofstream file(roomsFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << roomsFile << " for writing.\n";
        return;
    }
    for (const auto& r : rooms) {
        file << r.serialize() << "\n";
    }
    file.close();
}

Customer* HotelSystem::findCustomer(int id) {
    for (auto& c : customers) {
        if (c.getId() == id) return &c;
    }
    return nullptr;
}

Reservation* HotelSystem::findReservation(int id) {
    for (auto& r : reservations) {
        if (r.getId() == id) return &r;
    }
    return nullptr;
}

Room* HotelSystem::findRoom(int roomNumber) {
    for (auto& r : rooms) {
        if (r.getRoomNumber() == roomNumber) return &r;
    }
    return nullptr;
}

time_t HotelSystem::createDateTime(int year, int month, int day, int hour, int minute) {
    struct tm timeinfo = {};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = 0;
    timeinfo.tm_isdst = -1;
    return mktime(&timeinfo);
}

void HotelSystem::displayMenu() {
    std::cout << "\n========== SMART HOTEL RESERVATION SYSTEM ==========\n";
    std::cout << "1.  Add Customer\n";
    std::cout << "2.  View All Customers\n";
    std::cout << "3.  Modify Customer\n";
    std::cout << "4.  Delete Customer\n";
    std::cout << "5.  Create Reservation\n";
    std::cout << "6.  View All Reservations\n";
    std::cout << "7.  Cancel Reservation\n";
    std::cout << "8.  Check-In\n";
    std::cout << "9.  Check-Out & Generate Invoice\n";
    std::cout << "10. View All Rooms\n";
    std::cout << "11. View Room Schedule\n";
    std::cout << "12. View Complete Schedule\n";
    std::cout << "13. Admin Login\n";
    std::cout << "0.  Exit\n";
    std::cout << "====================================================\n";
    std::cout << "Enter choice: ";
}

void HotelSystem::displayAdminMenu() {
    std::cout << "\n========== ADMIN MENU ==========\n";
    std::cout << "1. View Occupancy Report\n";
    std::cout << "2. Create Backup\n";
    std::cout << "3. Restore Backup\n";
    std::cout << "4. Reschedule All Reservations\n";
    std::cout << "5. Logout\n";
    std::cout << "================================\n";
    std::cout << "Enter choice: ";
}


void HotelSystem::addCustomer() {
    std::string name, email, phone;

    std::cout << "\n=== Add New Customer ===\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // ----- Name -----
    while (true) {
        std::cout << "Name: ";
        std::getline(std::cin, name);

        if (!isValidName(name)) {
            std::cout << "Invalid name. Name cannot be empty or digits-only and must contain letters.\n";
            continue;
        }
        break;
    }

    // ----- Email -----
    while (true) {
        std::cout << "Email: ";
        std::getline(std::cin, email);

        if (!isValidEmail(email)) {
            std::cout << "Invalid email format. Example: user@example.com\n";
            continue;
        }
        break;
    }

    // ----- Phone -----
    while (true) {
        std::cout << "Phone: ";
        std::getline(std::cin, phone);

        if (!isValidPhone(phone)) {
            std::cout << "Invalid phone number. Use digits only and make sure the length is bigger than 7!.\n";
            continue;
        }
        break;
    }

    Customer c(name, email, phone);
    customers.push_back(c);

    std::cout << "\nCustomer added successfully! ID: " << c.getId() << "\n";
    saveCustomers();
}

void HotelSystem::viewCustomers() {
    std::cout << "\n========== ALL CUSTOMERS ==========\n";
    if (customers.empty()) {
        std::cout << "No customers found.\n";
        return;
    }
    
    for (const auto& c : customers) {
        std::cout << "---\n";
        c.display();
    }
}

void HotelSystem::modifyCustomer() {
    int id;
    std::cout << "\nEnter Customer ID to modify: ";

    if (!(std::cin >> id)) {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    Customer* c = findCustomer(id);
    if (!c) {
        std::cout << "Customer not found.\n";
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string name, email, phone;

    std::cout << "New Name (current: " << c->getName() << ", leave empty to keep): ";
    std::getline(std::cin, name);
    if (!name.empty()) {
        if (!isValidName(name)) {
            std::cout << "Invalid name. Keeping old name.\n";
        } else {
            c->setName(name);
        }
    }

    std::cout << "New Email (current: " << c->getEmail() << ", leave empty to keep): ";
    std::getline(std::cin, email);
    if (!email.empty()) {
        if (!isValidEmail(email)) {
            std::cout << "Invalid email. Keeping old email.\n";
        } else {
            c->setEmail(email);
        }
    }

    std::cout << "New Phone (current: " << c->getPhone() << ", leave empty to keep): ";
    std::getline(std::cin, phone);
    if (!phone.empty()) {
        if (!isValidPhone(phone)) {
            std::cout << "Invalid phone. Keeping old phone.\n";
        } else {
            c->setPhone(phone);
        }
    }

    std::cout << "Customer updated successfully.\n";
    saveCustomers();
}

void HotelSystem::deleteCustomer() {
    int id;
    std::cout << "\nEnter Customer ID to delete: ";
    
    if (!(std::cin >> id)) {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    auto it = std::find_if(customers.begin(), customers.end(),
                          [id](const Customer& c) { return c.getId() == id; });
    
    if (it != customers.end()) {
        customers.erase(it);
        std::cout << "Customer deleted successfully.\n";
        saveCustomers();
    } else {
        std::cout << "Customer not found.\n";
    }
}
static std::string roomTypeToString(RoomType t) {
    switch (t) {
        case RoomType::SINGLE: return "Single";
        case RoomType::DOUBLE: return "Double";
        case RoomType::SUITE:  return "Suite";
        case RoomType::DELUXE: return "Deluxe";
        default:               return "Unknown";
    }
}
bool isValidDate(int year, int month, int day, int hour) {
    if (month < 1 || month > 12) return false;
    if (hour < 0 || hour > 23) return false;

    int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    // Leap year check
    bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (month == 2 && leap)
        daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1])
        return false;

    return true;
}

void HotelSystem::createReservation() {
    int customerId;
    std::cout << "\n=== Create New Reservation ===\n";
    std::cout << "Customer ID: ";

    if (!(std::cin >> customerId)) {
        std::cout << "Invalid input for Customer ID.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    if (!findCustomer(customerId)) {
        std::cout << "Customer not found.\n";
        return;
    }

    // -------- NEW: choose room type ----------
    int typeChoice;
    RoomType desiredType;

    while (true) {
        std::cout << "Room Type (1. Single, 2. Double, 3. Suite, 4. Deluxe): ";
        if (!(std::cin >> typeChoice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }

        if (typeChoice == 1) { desiredType = RoomType::SINGLE; break; }
        if (typeChoice == 2) { desiredType = RoomType::DOUBLE; break; }
        if (typeChoice == 3) { desiredType = RoomType::SUITE; break; }
        if (typeChoice == 4) { desiredType = RoomType::DELUXE; break; }

        std::cout << "Invalid choice. Please select 1–4.\n";
    }

    // Check if hotel actually HAS that room type
    bool hasType = false;
    for (const auto& room : rooms) {
        if (room.getType() == desiredType) {
            hasType = true;
            break;
        }
    }

    if (!hasType) {
        std::cout << "Sorry, there are no rooms of this type.\n";
        return;
    }
    // -----------------------------------------

    int year, month, day, checkInHour;
    int checkOutYear, checkOutMonth, checkOutDay, checkOutHour;

    std::cout << "Check-in Date (YYYY MM DD HH): ";
    if (!(std::cin >> year >> month >> day >> checkInHour)) {
        std::cout << "Invalid input for check-in date/time.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "Check-out Date (YYYY MM DD HH): ";
    if (!(std::cin >> checkOutYear >> checkOutMonth >> checkOutDay >> checkOutHour)) {
        std::cout << "Invalid input for check-out date/time.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    if (!isValidDate(year, month, day, checkInHour)) {
    std::cout << "Invalid check-in date. Please enter a valid date.\n";
    return;
    }
    if (!isValidDate(checkOutYear, checkOutMonth, checkOutDay, checkOutHour)) {
    std::cout << "Invalid check-out date. Please enter a valid date.\n";
    return;
    }

    time_t checkIn = createDateTime(year, month, day, checkInHour, 0);
    time_t checkOut = createDateTime(checkOutYear, checkOutMonth, checkOutDay, checkOutHour, 0);

    time_t now = time(nullptr);
    if (checkIn < now) {
        std::cout << "Check-in date/time cannot be earlier than the current date/time.\n";
        return;
    }
    if (checkOut <= checkIn) {
        std::cout << "Invalid dates: check-out must be after check-in.\n";
        return;
    }

    // Create reservation inside vector
    reservations.emplace_back(customerId, checkIn, checkOut);
    Reservation& res = reservations.back();

    // Try to assign room of selected type
    bool scheduled = scheduler->scheduleReservation(res.getId(), desiredType);

if (scheduled) {
    std::cout << "\nReservation created and confirmed!\n";
    std::cout << "Reservation ID: " << res.getId() << "\n";
    std::cout << "Assigned Room: " << res.getAssignedRoomNumber() << "\n";

    // Optional: show room type based on the assigned room
    Room* room = findRoom(res.getAssignedRoomNumber());
    if (room) {
        std::cout << "Room Type: " << room->getTypeString() << "\n";
    }

    std::cout << "Duration: " << res.getDuration() << " night(s)\n";
    std::cout << "Total Cost: $" << res.getTotalCost() << "\n";
} else {
    std::cout << "\nReservation created but no "
              << roomTypeToString(desiredType)
              << " rooms are available for these dates.\n";
    std::cout << "Reservation ID: " << res.getId()
              << " (Status: Pending)\n";
}

    saveReservations();
    saveRooms();
}

void HotelSystem::viewReservations() {
    std::cout << "\n========== ALL RESERVATIONS ==========\n";
    if (reservations.empty()) {
        std::cout << "No reservations found.\n";
        return;
    }
    
    for (const auto& r : reservations) {
        std::cout << "---\n";
        r.display();
    }
}

void HotelSystem::cancelReservation() {
    int id;
    std::cout << "\nEnter Reservation ID to cancel: ";
    if (!(std::cin >> id)) {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    Reservation* r = findReservation(id);
    if (!r) {
        std::cout << "Reservation not found.\n";
        return;
    }
    
    if (r->getStatus() == ReservationStatus::CANCELLED) {
        std::cout << "Reservation already cancelled.\n";
        return;
    }
    
    scheduler->removeReservation(id);
    std::cout << "Reservation cancelled successfully.\n";
    
    saveReservations();
    saveRooms();
}

void HotelSystem::checkInReservation() {
    int id;
    std::cout << "\nEnter Reservation ID for check-in: ";
    if (!(std::cin >> id)) {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    Reservation* r = findReservation(id);
    if (!r) {
        std::cout << "Reservation not found.\n";
        return;
    }
    
    if (r->getStatus() != ReservationStatus::CONFIRMED) {
        std::cout << "Reservation must be confirmed before check-in.\n";
        return;
    }
    
    r->setStatus(ReservationStatus::CHECKED_IN);
    std::cout << "Check-in successful! Welcome!\n";
    std::cout << "Room Number: " << r->getAssignedRoomNumber() << "\n";
    
    saveReservations();
}

void HotelSystem::checkOutReservation() {
    int id;
    std::cout << "\nEnter Reservation ID for check-out: ";
    if (!(std::cin >> id)) {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    Reservation* r = findReservation(id);
    if (!r) {
        std::cout << "Reservation not found.\n";
        return;
    }
    
    if (r->getStatus() != ReservationStatus::CHECKED_IN) {
        std::cout << "Must be checked-in before check-out.\n";
        return;
    }
    
    r->setStatus(ReservationStatus::CHECKED_OUT);
    generateInvoice(id);
    
    saveReservations();
}


void HotelSystem::viewRooms() {
    std::cout << "\n========== ALL ROOMS ==========\n";
    for (const auto& r : rooms) {
        std::cout << "---\n";
        r.display();
    }
}

void HotelSystem::viewRoomSchedule() {
    int roomNum;
    std::cout << "\nEnter Room Number: ";
    if (!(std::cin >> roomNum)) {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    scheduler->displayRoomSchedule(roomNum);
}

void HotelSystem::generateInvoice(int id) {
    Reservation* r = findReservation(id);
    if (!r) {
        std::cout << "Reservation not found.\n";
        return;
    }
    
    Customer* c = findCustomer(r->getCustomerId());
    Room* room = findRoom(r->getAssignedRoomNumber());

    time_t invoiceDate = r->getCheckOutTime();
    time_t checkInTime = r->getCheckInTime();
    time_t checkOutTime = r->getCheckOutTime();
    
    std::cout << "\n";
    std::cout << "          SMART HOTEL - INVOICE                 \n";
    
    std::cout << "Invoice for Reservation #" << r->getId() << "\n";
    std::cout << "Date: " << std::put_time(std::localtime(&invoiceDate), "%Y-%m-%d") << "\n\n";
    
    std::cout << "Customer Details:\n";
    std::cout << "  Name: " << (c ? c->getName() : "N/A") << "\n";
    std::cout << "  Email: " << (c ? c->getEmail() : "N/A") << "\n";
    std::cout << "  Phone: " << (c ? c->getPhone() : "N/A") << "\n\n";
    
    std::cout << "Stay Details:\n";
    std::cout << "  Room Number: " << r->getAssignedRoomNumber() << "\n";
    std::cout << "  Room Type: " << (room ? room->getTypeString() : "N/A") << "\n";
    std::cout << "  Check-in: " << std::put_time(std::localtime(&checkInTime), "%Y-%m-%d %H:%M") << "\n";
    std::cout << "  Check-out: " << std::put_time(std::localtime(&checkOutTime), "%Y-%m-%d %H:%M") << "\n";
    std::cout << "  Duration: " << r->getDuration() << " night(s)\n\n";
    
    std::cout << "Charges:\n";
    std::cout << "  Room Rate: $" << (room ? room->getPricePerNight() : 0) << " per night\n";
    std::cout << "  Number of Nights: " << r->getDuration() << "\n";
    std::cout << "  Subtotal: $" << r->getTotalCost() << "\n";
    
    double tax = r->getTotalCost() * 0.10;
    double total = r->getTotalCost() + tax;
    
    std::cout << "  Tax (10%): $" << std::fixed << std::setprecision(2) << tax << "\n";
    std::cout << "  TOTAL: $" << total << "\n\n";
    
    std::cout << "Thank you for staying with us!\n";
}

bool HotelSystem::adminLogin() {
    std::string username, password;
    
    std::cout << "\n=== Admin Login ===\n";
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;
    
    if (username == adminUsername && password == adminPassword) {
        isAdminLoggedIn = true;
        std::cout << "Login successful!\n";
        return true;
    }
    
    std::cout << "Invalid credentials.\n";
    return false;
}

void HotelSystem::adminLogout() {
    isAdminLoggedIn = false;
    std::cout << "Logged out successfully.\n";
}

void HotelSystem::viewOccupancyReport() {
    std::cout << "\n========== OCCUPANCY REPORT ==========\n";
    std::cout << "Total Rooms: " << rooms.size() << "\n";
    std::cout << "Rooms in Use: " << scheduler->getTotalRoomsUsed() << "\n";
    std::cout << "Occupancy Rate: " << std::fixed << std::setprecision(1) 
              << scheduler->getOccupancyRate() << "%\n\n";
    
    int active = 0, pending = 0, checkedIn = 0, cancelled = 0;
    for (const auto& r : reservations) {
        switch (r.getStatus()) {
            case ReservationStatus::CONFIRMED: active++; break;
            case ReservationStatus::PENDING: pending++; break;
            case ReservationStatus::CHECKED_IN: checkedIn++; break;
            case ReservationStatus::CANCELLED: cancelled++; break;
            default: break;
        }
    }
    
    std::cout << "Reservation Statistics:\n";
    std::cout << "  Confirmed: " << active << "\n";
    std::cout << "  Checked In: " << checkedIn << "\n";
    std::cout << "  Pending: " << pending << "\n";
    std::cout << "  Cancelled: " << cancelled << "\n";
    std::cout << "  Total: " << reservations.size() << "\n";
}

void HotelSystem::createBackup() {
    std::cout << "\nCreating backup...\n";
    
    std::ifstream src1(customersFile, std::ios::binary);
    std::ofstream dst1("customers_backup.json", std::ios::binary);
    if (src1 && dst1) dst1 << src1.rdbuf();
    
    std::ifstream src2(reservationsFile, std::ios::binary);
    std::ofstream dst2("reservations_backup.json", std::ios::binary);
    if (src2 && dst2) dst2 << src2.rdbuf();
    
    std::ifstream src3(roomsFile, std::ios::binary);
    std::ofstream dst3("rooms_backup.json", std::ios::binary);
    if (src3 && dst3) dst3 << src3.rdbuf();
    
    std::cout << "Backup created successfully!\n";
}

void HotelSystem::restoreBackup() {
    std::cout << "\nRestoring from backup...\n";
    
    std::ifstream src1("customers_backup.json", std::ios::binary);
    std::ofstream dst1(customersFile, std::ios::binary);
    if (src1 && dst1) dst1 << src1.rdbuf();
    
    std::ifstream src2("reservations_backup.json", std::ios::binary);
    std::ofstream dst2(reservationsFile, std::ios::binary);
    if (src2 && dst2) dst2 << src2.rdbuf();
    
    std::ifstream src3("rooms_backup.json", std::ios::binary);
    std::ofstream dst3(roomsFile, std::ios::binary);
    if (src3 && dst3) dst3 << src3.rdbuf();
    
    loadData();
    std::cout << "Backup restored successfully!\n";
}

void HotelSystem::rescheduleAll() {
    std::cout << "\nRescheduling all reservations...\n";
    
    for (auto& room : rooms) {
        while (!room.getReservationIds().empty()) {
            int resId = room.getReservationIds().front();
            room.removeReservation(resId);
        }
    }
    
    for (auto& res : reservations) {
        if (res.getStatus() != ReservationStatus::CANCELLED &&
            res.getStatus() != ReservationStatus::CHECKED_OUT) {
            res.setAssignedRoomNumber(-1);
            res.setStatus(ReservationStatus::PENDING);
            res.setTotalCost(0.0);
        }
    }
    
    scheduler->scheduleReservations();
    
    std::cout << "Rescheduling complete!\n";
    saveReservations();
    saveRooms();
}

void HotelSystem::run() {
    int choice;
    
    while (true) {
        if (isAdminLoggedIn) {
            displayAdminMenu();
            
            if (!(std::cin >> choice)) {
                std::cout << "Invalid input. Please enter a number.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            
            switch (choice) {
                case 1: viewOccupancyReport(); break;
                case 2: createBackup(); break;
                case 3: restoreBackup(); break;
                case 4: rescheduleAll(); break;
                case 5: adminLogout(); break;
                default: std::cout << "Invalid choice.\n";
            }
        } else {
            displayMenu();
            
            if (!(std::cin >> choice)) {
                std::cout << "Invalid input. Please enter a number.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            
            switch (choice) {
                case 1: addCustomer(); break;
                case 2: viewCustomers(); break;
                case 3: modifyCustomer(); break;
                case 4: deleteCustomer(); break;
                case 5: createReservation(); break;
                case 6: viewReservations(); break;
                case 7: cancelReservation(); break;
                case 8: checkInReservation(); break;
                case 9: checkOutReservation(); break;
                case 10: viewRooms(); break;
                case 11: viewRoomSchedule(); break;
                case 12: scheduler->displaySchedule(); break;
                case 13: adminLogin(); break;
                case 0:
                    std::cout << "Thank you for using Smart Hotel System!\n";
                    return;
                default:
                    std::cout << "Invalid choice.\n";
            }
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cin.get();
    }
}
