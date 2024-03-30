//
// Created by Stanislau Senkevich on 21.3.24.
//

#ifndef LAB_5_PROJECT_VALIDATOR_H
#define LAB_5_PROJECT_VALIDATOR_H

#include "config.h"
#include "order.h"
#include "driver.h"
#include "dispatcher.h"
#include "exceptions.h"
#include "car.h"
#include <regex>
#include <ctime>
#include <chrono>
#include <sstream>
#include <openssl/evp.h>
#include <iomanip>

class Validator {
private:;
    static const std::regex name_pattern;
    static const std::regex address_pattern;
    static const std::regex city_pattern;
    static const std::regex date_pattern;
    static const std::regex license_pattern;
    static const std::regex login_pattern;
public:
    static bool validDispatcher(const Dispatcher& dispatcher);
    static bool validOrder(const Order& order, sqlite3* db);
    static bool validCar(const Car& car, sqlite3* db);
    static bool validDriver(const Driver& driver);
    static bool validLicense(const std::string& license);
    static bool validDate(const std::string& date);
    static bool validAge(const std::string& date);
    static bool validPeriod(const std::string& date_start, const std::string& date_end);

    static void validateUpdateCar(Car& update, int car_id, sqlite3* db);
    static void validateUpdateDriver(Driver& update, int driver_id, sqlite3* db);
    static void validateUpdateDispatcher(Dispatcher& update, int dispatcher_id, sqlite3* db);
    static void validateUpdateOrder(Order& update, int order_id, sqlite3* db);
    static void validateUpdateUser(User& update, int user_id, sqlite3* db);

    [[nodiscard]] static std::string hashPassword(const std::string& password) ;
    static bool validatePassword(const std::string& hash_pass, const std::string& provided_pass);
};

#endif //LAB_5_PROJECT_VALIDATOR_H
